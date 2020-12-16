#include <stdlib.h>
#include <stdio.h>

#include <obs/obs.h>

#include <iostream>
using namespace std;

const char *obs_module_errstr(int code);
const char *obs_video_errstr(int code);

int main(int argc, char *argv[]) {
	obs_startup("en-US", "/usr/lib64/obs-plugins", NULL);
	//obs_startup("en-US", "/usr/share/obs/obs-plugins", NULL);
	//cout << obs_get_version() << " " << obs_get_version_string() << endl;
	//cout << obs_initialized() << endl;

	obs_module_t *lincap;
	int rc = obs_open_module(&lincap, "/usr/lib64/obs-plugins/linux-capture.so", NULL);
	cout << "info: obs_open_module: " << obs_module_errstr(rc) << endl;
	rc = obs_init_module(lincap);
	cout << "info: obs_init_module: " << rc << endl;
	obs_post_load_modules();

	size_t idx = 0; const char *str;
	while (obs_enum_source_types(idx++, &str))
		printf("%s\n", str);

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

	obs_source_release(source);
	obs_scene_release(scene);
	return 0;

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

	obs_shutdown();
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
