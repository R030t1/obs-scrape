#include <stdlib.h>
#include <stdio.h>

#include <obs/obs.h>

#include <iostream>
#include <thread>
using namespace std;

void raw_video_callback(void *param, struct video_data *frame);

const char *obs_module_errstr(int code);
const char *obs_video_errstr(int code);

int main(int argc, char *argv[]) {
	int rc = 0;
	obs_startup(NULL, "/usr/lib64/obs-plugins", NULL);
	//obs_startup("en-US", "/usr/share/obs/obs-plugins", NULL);
	//cout << obs_get_version() << " " << obs_get_version_string() << endl;
	//cout << obs_initialized() << endl;

	obs_module_t *lincapmod;
	rc = obs_open_module(&lincapmod, "/usr/lib64/obs-plugins/linux-capture.so", NULL);
	cout << "info: obs_open_module: " << obs_module_errstr(rc) << endl;
	rc = obs_init_module(lincapmod);
	cout << "info: obs_init_module: " << rc << endl;

	obs_module_t *outputmod;
	rc = obs_open_module(&outputmod, "/usr/lib64/obs-plugins/obs-outputs.so", NULL);
	cout << "info: obs_open_module: " << obs_module_errstr(rc) << endl;
	rc = obs_init_module(outputmod);
	cout << "info: obs_init_module: " << rc << endl;

	obs_post_load_modules();

	cout << endl;
	size_t idx = 0; const char *str = nullptr;
	while (obs_enum_source_types(idx++, &str))
		printf("%s\n", str);

	cout << endl;
	idx = 0; str = nullptr;
	while (obs_enum_output_types(idx++, &str))
		printf("%s\n", str);
	cout << endl;

	obs_scene_t *scene = obs_scene_create("main");
	if (!scene) { cout << "error: obs_scene_create: failed" << endl; }

	obs_source_t *source = obs_source_create("xshm_input", "screen", nullptr, nullptr);
	if (!source) { cout << "error: obs_source_create: failed" << endl; }

	obs_sceneitem_t *scit = obs_scene_add(scene, source);
	if (!scit) { cout << "error: obs_scene_add: failed" << endl; }

	struct obs_transform_info itinfo;
	obs_sceneitem_get_info(scit, &itinfo);
	cout << "x: " << itinfo.pos.x << " y: " << itinfo.pos.y << " rot: " << itinfo.rot << endl;
	cout << "bounds.x: " << itinfo.bounds.x << " bounds.y: " << itinfo.bounds.y << endl;

	obs_add_raw_video_callback(NULL, raw_video_callback, NULL);

	obs_output_t *output = obs_output_create("null_output", "nullout", nullptr, nullptr);
	if (!output) { cout << "error: obs_output_create: failed" << endl; }

	cout << obs_output_get_width(output) << endl;
	cout << obs_output_get_height(output) << endl;
	if (!obs_output_start(output)) {
		cout << "error: obs_output_start: failed" << endl;
		cout << obs_output_get_last_error(output) << endl;
	}

	struct obs_video_info ovi = {
		.graphics_module = "libobs-opengl",
		.fps_num = 1,
		.fps_den = 60,
		.base_width = 1920,
		.base_height = 1080,
		.output_width = 1920,
		.output_height = 1080,
		.output_format = VIDEO_FORMAT_RGBA,
		.adapter = 0,
		.gpu_conversion = true,
		/* .colorspace = , */
		/* .range = , */
		/* .scale_type = , */
	};
	rc = obs_reset_video(&ovi);
	cout << "info: obs_reset_video: " << obs_video_errstr(rc) << endl;

	std::this_thread::sleep_for(5s);

	obs_output_release(output);
	obs_source_release(source);
	obs_scene_release(scene);
	obs_shutdown();
	return 0;
}

void raw_video_callback(void *param, struct video_data *frame) {
	printf(".\n");
}

#define NAME_SWITCH_CASE(element) case element: return #element
const char *obs_module_errstr(int code) {
	switch (code) {
	NAME_SWITCH_CASE(MODULE_SUCCESS);
	NAME_SWITCH_CASE(MODULE_ERROR);
	NAME_SWITCH_CASE(MODULE_FILE_NOT_FOUND);
	NAME_SWITCH_CASE(MODULE_MISSING_EXPORTS);
	NAME_SWITCH_CASE(MODULE_INCOMPATIBLE_VER);
	default: return "unknown return code";
	}
}

const char *obs_video_errstr(int code) {
	switch (code) {
	NAME_SWITCH_CASE(OBS_VIDEO_SUCCESS);
	NAME_SWITCH_CASE(OBS_VIDEO_NOT_SUPPORTED);
	NAME_SWITCH_CASE(OBS_VIDEO_INVALID_PARAM);
	NAME_SWITCH_CASE(OBS_VIDEO_CURRENTLY_ACTIVE);
	NAME_SWITCH_CASE(OBS_VIDEO_MODULE_NOT_FOUND);
	NAME_SWITCH_CASE(OBS_VIDEO_FAIL);
	default: return "unknown return code";
	}
}
