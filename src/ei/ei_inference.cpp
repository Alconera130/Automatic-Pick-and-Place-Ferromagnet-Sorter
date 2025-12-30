#include <Arduino.h>
#include <Object_Detection_inferencing.h>
#include "edge-impulse-sdk/dsp/image/image.hpp"
#include "globals.h"
#include "ei/ei_inference.h"
#include "vision/object_tracker.h"
#include <vector>

static int ei_camera_get_data(size_t offset, size_t length, float *out_ptr) {
	// we already have a RGB888 buffer, so recalculate offset into pixel index
	size_t pixel_ix = offset * 3;
	size_t pixels_left = length;
	size_t out_ptr_ix = 0;

	while (pixels_left != 0) {
		// Swap BGR to RGB here
		// due to https://github.com/espressif/esp32-camera/issues/379
		out_ptr[out_ptr_ix] = (snapshot_buf[pixel_ix + 2] << 16) + (snapshot_buf[pixel_ix + 1] << 8) + snapshot_buf[pixel_ix];

		// go to the next pixel
		out_ptr_ix++;
		pixel_ix += 3;
		pixels_left--;
	}
	// and done!
	return 0;
}

void runInference() {
    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) return;

    bool ok = fmt2rgb888(fb->buf, fb->len, PIXFORMAT_JPEG, snapshot_buf);

    ei::image::processing::resize_image(
        snapshot_buf,
        EI_CAMERA_RAW_FRAME_BUFFER_COLS,
        EI_CAMERA_RAW_FRAME_BUFFER_ROWS,
        snapshot_buf,
        EI_CLASSIFIER_INPUT_WIDTH,
        EI_CLASSIFIER_INPUT_HEIGHT,
        3
    );

    esp_camera_fb_return(fb);

    if (!ok) return;

    ei::signal_t signal;
    signal.total_length = EI_CLASSIFIER_INPUT_WIDTH * EI_CLASSIFIER_INPUT_HEIGHT;
    signal.get_data = &ei_camera_get_data;

    ei_impulse_result_t result;
    EI_IMPULSE_ERROR err = run_classifier(&signal, &result, false);
    if (err != EI_IMPULSE_OK) return;

    // print the predictions
    ei_printf("Predictions (DSP: %d ms., Classification: %d ms., Anomaly: %d ms.): \n",
                result.timing.dsp, result.timing.classification, result.timing.anomaly);

#if EI_CLASSIFIER_OBJECT_DETECTION == 1
    ei_printf("Object detection bounding boxes:\r\n");

    std::vector<DetectedObject> detectedObjects;
    
    for (uint32_t i = 0; i < result.bounding_boxes_count; i++) {
        ei_impulse_result_bounding_box_t bb = result.bounding_boxes[i];

        if (bb.value < confidence) {
        continue;
        }

        ei_printf("  %s (%f) [ x: %u, y: %u, width: %u, height: %u ]\r\n",
                bb.label,
                bb.value,
                bb.x,
                bb.y,
                bb.width,
                bb.height);

        int cx = bb.x + bb.width / 2;
        int cy = bb.y + bb.height / 2;

        int imgW = EI_CLASSIFIER_INPUT_WIDTH;
        int imgH = EI_CLASSIFIER_INPUT_HEIGHT;
        if (hmirror_enabled) cx = imgW - cx;
        if (vflip_enabled)  cy = imgH - cy;

        float robotX_mm = 0.0f, robotY_mm = 0.0f;
        cameraToRobot(cx, cy, robotX_mm, robotY_mm);

        detectedObjects.push_back(DetectedObject(robotX_mm, robotY_mm, String(bb.label)));
    }

    DetectedObject closest = findClosestToOrigin(detectedObjects);
    if (closest.x != -1 && closest.y != -1) {
        ei_printf("Closest object to origin: %s at (%.1f, %.1f) mm\n", closest.label.c_str(), closest.x, closest.y);
        pickAndPlace(closest.x, closest.y);
    } else {
        ei_printf("No valid objects detected for pick and place.\n");
    }

    // Clear vector for next frame
    detectedObjects.clear();

    // Print the prediction results (classification)
#else
    ei_printf("Predictions:\r\n");
    for (uint16_t i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++) {
        ei_printf("  %s: ", ei_classifier_inferencing_categories[i]);
        ei_printf("%.5f\r\n", result.classification[i].value);
    }
#endif

    // Print anomaly result (if it exists)
#if EI_CLASSIFIER_HAS_ANOMALY
    ei_printf("Anomaly prediction: %.3f\r\n", result.anomaly);
#endif

#if EI_CLASSIFIER_HAS_VISUAL_ANOMALY
    ei_printf("Visual anomalies:\r\n");
    for (uint32_t i = 0; i < result.visual_ad_count; i++) {
        ei_impulse_result_bounding_box_t bb = result.visual_ad_grid_cells[i];
        if (bb.value == 0) {
        continue;
        }
        ei_printf("  %s (%f) [ x: %u, y: %u, width: %u, height: %u ]\r\n",
                bb.label,
                bb.value,
                bb.x,
                bb.y,
                bb.width,
                bb.height);
    }

#endif
}