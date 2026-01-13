#include "XRUtils.hpp"
#include "Utils/Utils.hpp" 

void dirtyXRErrorPrintInternal(XrResult result, XrInstance instance, const char* file, int line)
{
    if (instance != XR_NULL_HANDLE)
    {
        char buffer[XR_MAX_RESULT_STRING_SIZE];
        XrResult string_result = xrResultToString(instance, result, buffer);
        if (string_result == XR_SUCCESS)
        {
            logMessageInternal(1, std::string("OpenXR Error: ") + buffer + " (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
            return;
        }
    }

    switch (result)
    {
    // Success Codes
    case XR_SUCCESS:
        logMessageInternal(3, "XR_SUCCESS: Function successfully completed. (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
    case XR_TIMEOUT_EXPIRED:
        logMessageInternal(1, "XR_TIMEOUT_EXPIRED: The specified timeout time occurred before the operation could complete. (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
    case XR_SESSION_LOSS_PENDING:
        logMessageInternal(1, "XR_SESSION_LOSS_PENDING: The session will be lost soon. (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
    case XR_EVENT_UNAVAILABLE:
        logMessageInternal(1, "XR_EVENT_UNAVAILABLE: No event was available. (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
    case XR_SPACE_BOUNDS_UNAVAILABLE:
        logMessageInternal(1, "XR_SPACE_BOUNDS_UNAVAILABLE: The space's bounds are not known at the moment. (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
    case XR_SESSION_NOT_FOCUSED:
        logMessageInternal(1, "XR_SESSION_NOT_FOCUSED: The session is not in the focused state. (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
    case XR_FRAME_DISCARDED:
        logMessageInternal(1, "XR_FRAME_DISCARDED: A frame has been discarded from composition. (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;

    // Additional extension success/warning codes
#ifdef XR_RENDER_MODEL_UNAVAILABLE_FB
    case XR_RENDER_MODEL_UNAVAILABLE_FB:
        logMessageInternal(1, "XR_RENDER_MODEL_UNAVAILABLE_FB: The model is unavailable. (Added by the XR_FB_render_model extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_FB_render_model"}, file, line);
        break;
#endif

#ifdef XR_SCENE_MARKER_DATA_NOT_STRING_MSFT
    case XR_SCENE_MARKER_DATA_NOT_STRING_MSFT:
        logMessageInternal(1, "XR_SCENE_MARKER_DATA_NOT_STRING_MSFT: Marker does not encode a string. (Added by the XR_MSFT_scene_marker extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_MSFT_scene_marker"}, file, line);
        break;
#endif

#ifdef XR_ENVIRONMENT_DEPTH_NOT_AVAILABLE_META
    case XR_ENVIRONMENT_DEPTH_NOT_AVAILABLE_META:
        logMessageInternal(1, "XR_ENVIRONMENT_DEPTH_NOT_AVAILABLE_META: Warning: The requested depth image is not yet available. (Added by the XR_META_environment_depth extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_META_environment_depth"}, file, line);
        break;
#endif

#ifdef XR_COLOCATION_DISCOVERY_ALREADY_ADVERTISING_META
    case XR_COLOCATION_DISCOVERY_ALREADY_ADVERTISING_META:
        logMessageInternal(1, "XR_COLOCATION_DISCOVERY_ALREADY_ADVERTISING_META: Colocation advertisement has already been enabled (Added by the XR_META_colocation_discovery extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_META_colocation_discovery"}, file, line);
        break;
#endif

#ifdef XR_COLOCATION_DISCOVERY_ALREADY_DISCOVERING_META
    case XR_COLOCATION_DISCOVERY_ALREADY_DISCOVERING_META:
        logMessageInternal(1, "XR_COLOCATION_DISCOVERY_ALREADY_DISCOVERING_META: Colocation discovery has already been enabled (Added by the XR_META_colocation_discovery extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_META_colocation_discovery"}, file, line);
        break;
#endif

    // Error Codes
    case XR_ERROR_VALIDATION_FAILURE:
        logMessageInternal(1, "XR_ERROR_VALIDATION_FAILURE: The function usage was invalid in some way. (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
    case XR_ERROR_RUNTIME_FAILURE:
        logMessageInternal(1, "XR_ERROR_RUNTIME_FAILURE: The runtime failed to handle the function in an unexpected way. (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
    case XR_ERROR_OUT_OF_MEMORY:
        logMessageInternal(1, "XR_ERROR_OUT_OF_MEMORY: A memory allocation has failed. (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
    case XR_ERROR_API_VERSION_UNSUPPORTED:
        logMessageInternal(1, "XR_ERROR_API_VERSION_UNSUPPORTED: The runtime does not support the requested API version. (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
    case XR_ERROR_INITIALIZATION_FAILED:
        logMessageInternal(1, "XR_ERROR_INITIALIZATION_FAILED: Initialization of object could not be completed. (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
    case XR_ERROR_FUNCTION_UNSUPPORTED:
        logMessageInternal(1, "XR_ERROR_FUNCTION_UNSUPPORTED: The requested function was not found or is otherwise unsupported. (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
    case XR_ERROR_FEATURE_UNSUPPORTED:
        logMessageInternal(1, "XR_ERROR_FEATURE_UNSUPPORTED: The requested feature is not supported. (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
    case XR_ERROR_EXTENSION_NOT_PRESENT:
        logMessageInternal(1, "XR_ERROR_EXTENSION_NOT_PRESENT: A requested extension is not supported. (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
    case XR_ERROR_LIMIT_REACHED:
        logMessageInternal(1, "XR_ERROR_LIMIT_REACHED: The runtime supports no more of the requested resource. (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
    case XR_ERROR_SIZE_INSUFFICIENT:
        logMessageInternal(1, "XR_ERROR_SIZE_INSUFFICIENT: The supplied size was smaller than required. (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
    case XR_ERROR_HANDLE_INVALID:
        logMessageInternal(1, "XR_ERROR_HANDLE_INVALID: A supplied object handle was invalid. (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
    case XR_ERROR_INSTANCE_LOST:
        logMessageInternal(1, "XR_ERROR_INSTANCE_LOST: The XrInstance was lost or could not be found. (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
    case XR_ERROR_SESSION_RUNNING:
        logMessageInternal(1, "XR_ERROR_SESSION_RUNNING: The session is already running. (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
    case XR_ERROR_SESSION_NOT_RUNNING:
        logMessageInternal(1, "XR_ERROR_SESSION_NOT_RUNNING: The session is not yet running. (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
    case XR_ERROR_SESSION_LOST:
        logMessageInternal(1, "XR_ERROR_SESSION_LOST: The XrSession was lost. (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
    case XR_ERROR_SYSTEM_INVALID:
        logMessageInternal(1, "XR_ERROR_SYSTEM_INVALID: The provided XrSystemId was invalid. (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
    case XR_ERROR_PATH_INVALID:
        logMessageInternal(1, "XR_ERROR_PATH_INVALID: The provided XrPath was not valid. (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
    case XR_ERROR_PATH_COUNT_EXCEEDED:
        logMessageInternal(1, "XR_ERROR_PATH_COUNT_EXCEEDED: The maximum number of supported semantic paths has been reached. (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
    case XR_ERROR_PATH_FORMAT_INVALID:
        logMessageInternal(1, "XR_ERROR_PATH_FORMAT_INVALID: The semantic path character format is invalid. (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
    case XR_ERROR_PATH_UNSUPPORTED:
        logMessageInternal(1, "XR_ERROR_PATH_UNSUPPORTED: The semantic path is unsupported. (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
    case XR_ERROR_LAYER_INVALID:
        logMessageInternal(1, "XR_ERROR_LAYER_INVALID: The layer was NULL or otherwise invalid. (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
    case XR_ERROR_LAYER_LIMIT_EXCEEDED:
        logMessageInternal(1, "XR_ERROR_LAYER_LIMIT_EXCEEDED: The number of specified layers is greater than supported. (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
    case XR_ERROR_SWAPCHAIN_RECT_INVALID:
        logMessageInternal(1, "XR_ERROR_SWAPCHAIN_RECT_INVALID: The image rect was negatively sized or otherwise invalid. (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
    case XR_ERROR_SWAPCHAIN_FORMAT_UNSUPPORTED:
        logMessageInternal(1, "XR_ERROR_SWAPCHAIN_FORMAT_UNSUPPORTED: The image format is not supported by the runtime or platform. (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
    case XR_ERROR_ACTION_TYPE_MISMATCH:
        logMessageInternal(1, "XR_ERROR_ACTION_TYPE_MISMATCH: The API used to retrieve an action's state does not match the action's type. (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
    case XR_ERROR_SESSION_NOT_READY:
        logMessageInternal(1, "XR_ERROR_SESSION_NOT_READY: The session is not in the ready state. (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
    case XR_ERROR_SESSION_NOT_STOPPING:
        logMessageInternal(1, "XR_ERROR_SESSION_NOT_STOPPING: The session is not in the stopping state. (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
    case XR_ERROR_TIME_INVALID:
        logMessageInternal(1, "XR_ERROR_TIME_INVALID: The provided XrTime was zero, negative, or out of range. (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
    case XR_ERROR_REFERENCE_SPACE_UNSUPPORTED:
        logMessageInternal(1, "XR_ERROR_REFERENCE_SPACE_UNSUPPORTED: The specified reference space is not supported. (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
    case XR_ERROR_FILE_ACCESS_ERROR:
        logMessageInternal(1, "XR_ERROR_FILE_ACCESS_ERROR: The file could not be accessed. (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
    case XR_ERROR_FILE_CONTENTS_INVALID:
        logMessageInternal(1, "XR_ERROR_FILE_CONTENTS_INVALID: The file's contents were invalid. (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
    case XR_ERROR_FORM_FACTOR_UNSUPPORTED:
        logMessageInternal(1, "XR_ERROR_FORM_FACTOR_UNSUPPORTED: The specified form factor is not supported. (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
    case XR_ERROR_FORM_FACTOR_UNAVAILABLE:
        logMessageInternal(1, "XR_ERROR_FORM_FACTOR_UNAVAILABLE: The specified form factor is supported but device is unavailable. (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
    case XR_ERROR_API_LAYER_NOT_PRESENT:
        logMessageInternal(1, "XR_ERROR_API_LAYER_NOT_PRESENT: A requested API layer is not present or could not be loaded. (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
    case XR_ERROR_CALL_ORDER_INVALID:
        logMessageInternal(1, "XR_ERROR_CALL_ORDER_INVALID: The call was made without a previously required call. (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
    case XR_ERROR_GRAPHICS_DEVICE_INVALID:
        logMessageInternal(1, "XR_ERROR_GRAPHICS_DEVICE_INVALID: The given graphics device is not in a valid state. (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
    case XR_ERROR_POSE_INVALID:
        logMessageInternal(1, "XR_ERROR_POSE_INVALID: The supplied pose was invalid with respect to the requirements. (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
    case XR_ERROR_INDEX_OUT_OF_RANGE:
        logMessageInternal(1, "XR_ERROR_INDEX_OUT_OF_RANGE: The supplied index was outside the range of valid indices. (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
    case XR_ERROR_VIEW_CONFIGURATION_TYPE_UNSUPPORTED:
        logMessageInternal(1, "XR_ERROR_VIEW_CONFIGURATION_TYPE_UNSUPPORTED: The specified view configuration type is not supported. (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
    case XR_ERROR_ENVIRONMENT_BLEND_MODE_UNSUPPORTED:
        logMessageInternal(1, "XR_ERROR_ENVIRONMENT_BLEND_MODE_UNSUPPORTED: The specified environment blend mode is not supported. (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
    case XR_ERROR_NAME_DUPLICATED:
        logMessageInternal(1, "XR_ERROR_NAME_DUPLICATED: The name provided was a duplicate of an already-existing resource. (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
    case XR_ERROR_NAME_INVALID:
        logMessageInternal(1, "XR_ERROR_NAME_INVALID: The name provided was invalid. (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
    case XR_ERROR_ACTIONSET_NOT_ATTACHED:
        logMessageInternal(1, "XR_ERROR_ACTIONSET_NOT_ATTACHED: A referenced action set is not attached to the session. (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
    case XR_ERROR_ACTIONSETS_ALREADY_ATTACHED:
        logMessageInternal(1, "XR_ERROR_ACTIONSETS_ALREADY_ATTACHED: The session already has attached action sets. (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
    case XR_ERROR_LOCALIZED_NAME_DUPLICATED:
        logMessageInternal(1, "XR_ERROR_LOCALIZED_NAME_DUPLICATED: The localized name was a duplicate of an existing resource. (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
    case XR_ERROR_LOCALIZED_NAME_INVALID:
        logMessageInternal(1, "XR_ERROR_LOCALIZED_NAME_INVALID: The localized name provided was invalid. (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
    case XR_ERROR_GRAPHICS_REQUIREMENTS_CALL_MISSING:
        logMessageInternal(1, "XR_ERROR_GRAPHICS_REQUIREMENTS_CALL_MISSING: xrGet*GraphicsRequirements was not called before xrCreateSession. (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
    case XR_ERROR_RUNTIME_UNAVAILABLE:
        logMessageInternal(1, "XR_ERROR_RUNTIME_UNAVAILABLE: The loader was unable to find or load a runtime. (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
    case XR_ERROR_EXTENSION_DEPENDENCY_NOT_ENABLED:
        logMessageInternal(1, "XR_ERROR_EXTENSION_DEPENDENCY_NOT_ENABLED: Extension dependencies are not enabled. (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
    case XR_ERROR_PERMISSION_INSUFFICIENT:
        logMessageInternal(1, "XR_ERROR_PERMISSION_INSUFFICIENT: Insufficient permissions. (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;

    // Additional extension error codes
#ifdef XR_ERROR_ANDROID_THREAD_SETTINGS_ID_INVALID_KHR
    case XR_ERROR_ANDROID_THREAD_SETTINGS_ID_INVALID_KHR:
        logMessageInternal(1, "XR_ERROR_ANDROID_THREAD_SETTINGS_ID_INVALID_KHR: xrSetAndroidApplicationThreadKHR failed as thread id is invalid. (Added by the XR_KHR_android_thread_settings extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_KHR_android_thread_settings"}, file, line);
        break;
#endif

#ifdef XR_ERROR_ANDROID_THREAD_SETTINGS_FAILURE_KHR
    case XR_ERROR_ANDROID_THREAD_SETTINGS_FAILURE_KHR:
        logMessageInternal(1, "XR_ERROR_ANDROID_THREAD_SETTINGS_FAILURE_KHR: xrSetAndroidApplicationThreadKHR failed setting the thread attributes/priority. (Added by the XR_KHR_android_thread_settings extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_KHR_android_thread_settings"}, file, line);
        break;
#endif

#ifdef XR_ERROR_CREATE_SPATIAL_ANCHOR_FAILED_MSFT
    case XR_ERROR_CREATE_SPATIAL_ANCHOR_FAILED_MSFT:
        logMessageInternal(1, "XR_ERROR_CREATE_SPATIAL_ANCHOR_FAILED_MSFT: Spatial anchor could not be created at that location. (Added by the XR_MSFT_spatial_anchor extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_MSFT_spatial_anchor"}, file, line);
        break;
#endif

#ifdef XR_ERROR_SECONDARY_VIEW_CONFIGURATION_TYPE_NOT_ENABLED_MSFT
    case XR_ERROR_SECONDARY_VIEW_CONFIGURATION_TYPE_NOT_ENABLED_MSFT:
        logMessageInternal(1, "XR_ERROR_SECONDARY_VIEW_CONFIGURATION_TYPE_NOT_ENABLED_MSFT: The secondary view configuration was not enabled when creating the session. (Added by the XR_MSFT_secondary_view_configuration extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_MSFT_secondary_view_configuration"}, file, line);
        break;
#endif

#ifdef XR_ERROR_CONTROLLER_MODEL_KEY_INVALID_MSFT
    case XR_ERROR_CONTROLLER_MODEL_KEY_INVALID_MSFT:
        logMessageInternal(1, "XR_ERROR_CONTROLLER_MODEL_KEY_INVALID_MSFT: The controller model key is invalid. (Added by the XR_MSFT_controller_model extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_MSFT_controller_model"}, file, line);
        break;
#endif

#ifdef XR_ERROR_REPROJECTION_MODE_UNSUPPORTED_MSFT
    case XR_ERROR_REPROJECTION_MODE_UNSUPPORTED_MSFT:
        logMessageInternal(1, "XR_ERROR_REPROJECTION_MODE_UNSUPPORTED_MSFT: The reprojection mode is not supported. (Added by the XR_MSFT_composition_layer_reprojection extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_MSFT_composition_layer_reprojection"}, file, line);
        break;
#endif

#ifdef XR_ERROR_COMPUTE_NEW_SCENE_NOT_COMPLETED_MSFT
    case XR_ERROR_COMPUTE_NEW_SCENE_NOT_COMPLETED_MSFT:
        logMessageInternal(1, "XR_ERROR_COMPUTE_NEW_SCENE_NOT_COMPLETED_MSFT: Compute new scene not completed. (Added by the XR_MSFT_scene_understanding extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_MSFT_scene_understanding"}, file, line);
        break;
#endif

#ifdef XR_ERROR_SCENE_COMPONENT_ID_INVALID_MSFT
    case XR_ERROR_SCENE_COMPONENT_ID_INVALID_MSFT:
        logMessageInternal(1, "XR_ERROR_SCENE_COMPONENT_ID_INVALID_MSFT: Scene component id invalid. (Added by the XR_MSFT_scene_understanding extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_MSFT_scene_understanding"}, file, line);
        break;
#endif

#ifdef XR_ERROR_SCENE_COMPONENT_TYPE_MISMATCH_MSFT
    case XR_ERROR_SCENE_COMPONENT_TYPE_MISMATCH_MSFT:
        logMessageInternal(1, "XR_ERROR_SCENE_COMPONENT_TYPE_MISMATCH_MSFT: Scene component type mismatch. (Added by the XR_MSFT_scene_understanding extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_MSFT_scene_understanding"}, file, line);
        break;
#endif

#ifdef XR_ERROR_SCENE_MESH_BUFFER_ID_INVALID_MSFT
    case XR_ERROR_SCENE_MESH_BUFFER_ID_INVALID_MSFT:
        logMessageInternal(1, "XR_ERROR_SCENE_MESH_BUFFER_ID_INVALID_MSFT: Scene mesh buffer id invalid. (Added by the XR_MSFT_scene_understanding extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_MSFT_scene_understanding"}, file, line);
        break;
#endif

#ifdef XR_ERROR_SCENE_COMPUTE_FEATURE_INCOMPATIBLE_MSFT
    case XR_ERROR_SCENE_COMPUTE_FEATURE_INCOMPATIBLE_MSFT:
        logMessageInternal(1, "XR_ERROR_SCENE_COMPUTE_FEATURE_INCOMPATIBLE_MSFT: Scene compute feature incompatible. (Added by the XR_MSFT_scene_understanding extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_MSFT_scene_understanding"}, file, line);
        break;
#endif

#ifdef XR_ERROR_SCENE_COMPUTE_CONSISTENCY_MISMATCH_MSFT
    case XR_ERROR_SCENE_COMPUTE_CONSISTENCY_MISMATCH_MSFT:
        logMessageInternal(1, "XR_ERROR_SCENE_COMPUTE_CONSISTENCY_MISMATCH_MSFT: Scene compute consistency mismatch. (Added by the XR_MSFT_scene_understanding extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_MSFT_scene_understanding"}, file, line);
        break;
#endif

#ifdef XR_ERROR_DISPLAY_REFRESH_RATE_UNSUPPORTED_FB
    case XR_ERROR_DISPLAY_REFRESH_RATE_UNSUPPORTED_FB:
        logMessageInternal(1, "XR_ERROR_DISPLAY_REFRESH_RATE_UNSUPPORTED_FB: The display refresh rate is not supported by the platform. (Added by the XR_FB_display_refresh_rate extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_FB_display_refresh_rate"}, file, line);
        break;
#endif

#ifdef XR_ERROR_COLOR_SPACE_UNSUPPORTED_FB
    case XR_ERROR_COLOR_SPACE_UNSUPPORTED_FB:
        logMessageInternal(1, "XR_ERROR_COLOR_SPACE_UNSUPPORTED_FB: The color space is not supported by the runtime. (Added by the XR_FB_color_space extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_FB_color_space"}, file, line);
        break;
#endif

#ifdef XR_ERROR_SPACE_COMPONENT_NOT_SUPPORTED_FB
    case XR_ERROR_SPACE_COMPONENT_NOT_SUPPORTED_FB:
        logMessageInternal(1, "XR_ERROR_SPACE_COMPONENT_NOT_SUPPORTED_FB: The component type is not supported for this space. (Added by the XR_FB_spatial_entity extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_FB_spatial_entity"}, file, line);
        break;
#endif

#ifdef XR_ERROR_SPACE_COMPONENT_NOT_ENABLED_FB
    case XR_ERROR_SPACE_COMPONENT_NOT_ENABLED_FB:
        logMessageInternal(1, "XR_ERROR_SPACE_COMPONENT_NOT_ENABLED_FB: The required component is not enabled for this space. (Added by the XR_FB_spatial_entity extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_FB_spatial_entity"}, file, line);
        break;
#endif

#ifdef XR_ERROR_SPACE_COMPONENT_STATUS_PENDING_FB
    case XR_ERROR_SPACE_COMPONENT_STATUS_PENDING_FB:
        logMessageInternal(1, "XR_ERROR_SPACE_COMPONENT_STATUS_PENDING_FB: A request to set the component’s status is currently pending. (Added by the XR_FB_spatial_entity extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_FB_spatial_entity"}, file, line);
        break;
#endif

#ifdef XR_ERROR_SPACE_COMPONENT_STATUS_ALREADY_SET_FB
    case XR_ERROR_SPACE_COMPONENT_STATUS_ALREADY_SET_FB:
        logMessageInternal(1, "XR_ERROR_SPACE_COMPONENT_STATUS_ALREADY_SET_FB: The component is already set to the requested value. (Added by the XR_FB_spatial_entity extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_FB_spatial_entity"}, file, line);
        break;
#endif

#ifdef XR_ERROR_UNEXPECTED_STATE_PASSTHROUGH_FB
    case XR_ERROR_UNEXPECTED_STATE_PASSTHROUGH_FB:
        logMessageInternal(1, "XR_ERROR_UNEXPECTED_STATE_PASSTHROUGH_FB: The object state is unexpected for the issued command. (Added by the XR_FB_passthrough extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_FB_passthrough"}, file, line);
        break;
#endif

#ifdef XR_ERROR_FEATURE_ALREADY_CREATED_PASSTHROUGH_FB
    case XR_ERROR_FEATURE_ALREADY_CREATED_PASSTHROUGH_FB:
        logMessageInternal(1, "XR_ERROR_FEATURE_ALREADY_CREATED_PASSTHROUGH_FB: Trying to create an MR feature when one was already created and only one instance is allowed. (Added by the XR_FB_passthrough extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_FB_passthrough"}, file, line);
        break;
#endif

#ifdef XR_ERROR_FEATURE_REQUIRED_PASSTHROUGH_FB
    case XR_ERROR_FEATURE_REQUIRED_PASSTHROUGH_FB:
        logMessageInternal(1, "XR_ERROR_FEATURE_REQUIRED_PASSTHROUGH_FB: Requested functionality requires a feature to be created first. (Added by the XR_FB_passthrough extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_FB_passthrough"}, file, line);
        break;
#endif

#ifdef XR_ERROR_NOT_PERMITTED_PASSTHROUGH_FB
    case XR_ERROR_NOT_PERMITTED_PASSTHROUGH_FB:
        logMessageInternal(1, "XR_ERROR_NOT_PERMITTED_PASSTHROUGH_FB: Requested functionality is not permitted - application is not allowed to perform the requested operation. (Added by the XR_FB_passthrough extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_FB_passthrough"}, file, line);
        break;
#endif

#ifdef XR_ERROR_INSUFFICIENT_RESOURCES_PASSTHROUGH_FB
    case XR_ERROR_INSUFFICIENT_RESOURCES_PASSTHROUGH_FB:
        logMessageInternal(1, "XR_ERROR_INSUFFICIENT_RESOURCES_PASSTHROUGH_FB: There were insufficient resources available to perform an operation. (Added by the XR_FB_passthrough extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_FB_passthrough"}, file, line);
        break;
#endif

#ifdef XR_ERROR_UNKNOWN_PASSTHROUGH_FB
    case XR_ERROR_UNKNOWN_PASSTHROUGH_FB:
        logMessageInternal(1, "XR_ERROR_UNKNOWN_PASSTHROUGH_FB: Unknown Passthrough error (no further details provided). (Added by the XR_FB_passthrough extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_FB_passthrough"}, file, line);
        break;
#endif

#ifdef XR_ERROR_RENDER_MODEL_KEY_INVALID_FB
    case XR_ERROR_RENDER_MODEL_KEY_INVALID_FB:
        logMessageInternal(1, "XR_ERROR_RENDER_MODEL_KEY_INVALID_FB: The model key is invalid. (Added by the XR_FB_render_model extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_FB_render_model"}, file, line);
        break;
#endif

#ifdef XR_ERROR_MARKER_NOT_TRACKED_VARJO
    case XR_ERROR_MARKER_NOT_TRACKED_VARJO:
        logMessageInternal(1, "XR_ERROR_MARKER_NOT_TRACKED_VARJO: Marker tracking is disabled or the specified marker is not currently tracked. (Added by the XR_VARJO_marker_tracking extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_VARJO_marker_tracking"}, file, line);
        break;
#endif

#ifdef XR_ERROR_MARKER_ID_INVALID_VARJO
    case XR_ERROR_MARKER_ID_INVALID_VARJO:
        logMessageInternal(1, "XR_ERROR_MARKER_ID_INVALID_VARJO: The specified marker ID is not valid. (Added by the XR_VARJO_marker_tracking extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_VARJO_marker_tracking"}, file, line);
        break;
#endif

#ifdef XR_ERROR_MARKER_DETECTOR_PERMISSION_DENIED_ML
    case XR_ERROR_MARKER_DETECTOR_PERMISSION_DENIED_ML:
        logMessageInternal(1, "XR_ERROR_MARKER_DETECTOR_PERMISSION_DENIED_ML: The com.magicleap.permission.MARKER_TRACKING permission was denied. (Added by the XR_ML_marker_understanding extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_ML_marker_understanding"}, file, line);
        break;
#endif

#ifdef XR_ERROR_MARKER_DETECTOR_LOCATE_FAILED_ML
    case XR_ERROR_MARKER_DETECTOR_LOCATE_FAILED_ML:
        logMessageInternal(1, "XR_ERROR_MARKER_DETECTOR_LOCATE_FAILED_ML: The specified marker could not be located spatially. (Added by the XR_ML_marker_understanding extension) (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
#endif

#ifdef XR_ERROR_MARKER_DETECTOR_INVALID_DATA_QUERY_ML
    case XR_ERROR_MARKER_DETECTOR_INVALID_DATA_QUERY_ML:
        logMessageInternal(1, "XR_ERROR_MARKER_DETECTOR_INVALID_DATA_QUERY_ML: The marker queried does not contain data of the requested type. (Added by the XR_ML_marker_understanding extension) (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
#endif

#ifdef XR_ERROR_MARKER_DETECTOR_INVALID_CREATE_INFO_ML
    case XR_ERROR_MARKER_DETECTOR_INVALID_CREATE_INFO_ML:
        logMessageInternal(1, "XR_ERROR_MARKER_DETECTOR_INVALID_CREATE_INFO_ML: createInfo contains mutually exclusive parameters. (Added by the XR_ML_marker_understanding extension) (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
#endif

#ifdef XR_ERROR_MARKER_INVALID_ML
    case XR_ERROR_MARKER_INVALID_ML:
        logMessageInternal(1, "XR_ERROR_MARKER_INVALID_ML: The marker id passed to the function was invalid. (Added by the XR_ML_marker_understanding extension) (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
#endif

#ifdef XR_ERROR_LOCALIZATION_MAP_INCOMPATIBLE_ML
    case XR_ERROR_LOCALIZATION_MAP_INCOMPATIBLE_ML:
        logMessageInternal(1, "XR_ERROR_LOCALIZATION_MAP_INCOMPATIBLE_ML: The localization map being imported is not compatible with current OS or mode. (Added by the XR_ML_localization_map extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_ML_localization_map"}, file, line);
        break;
#endif

#ifdef XR_ERROR_LOCALIZATION_MAP_UNAVAILABLE_ML
    case XR_ERROR_LOCALIZATION_MAP_UNAVAILABLE_ML:
        logMessageInternal(1, "XR_ERROR_LOCALIZATION_MAP_UNAVAILABLE_ML: The localization map requested is not available. (Added by the XR_ML_localization_map extension) (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
#endif

#ifdef XR_ERROR_LOCALIZATION_MAP_FAIL_ML
    case XR_ERROR_LOCALIZATION_MAP_FAIL_ML:
        logMessageInternal(1, "XR_ERROR_LOCALIZATION_MAP_FAIL_ML: The map localization service failed to fulfill the request, retry later. (Added by the XR_ML_localization_map extension) (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
#endif

#ifdef XR_ERROR_LOCALIZATION_MAP_IMPORT_EXPORT_PERMISSION_DENIED_ML
    case XR_ERROR_LOCALIZATION_MAP_IMPORT_EXPORT_PERMISSION_DENIED_ML:
        logMessageInternal(1, "XR_ERROR_LOCALIZATION_MAP_IMPORT_EXPORT_PERMISSION_DENIED_ML: The com.magicleap.permission.SPACE_IMPORT_EXPORT permission was denied. (Added by the XR_ML_localization_map extension) (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
#endif

#ifdef XR_ERROR_LOCALIZATION_MAP_PERMISSION_DENIED_ML
    case XR_ERROR_LOCALIZATION_MAP_PERMISSION_DENIED_ML:
        logMessageInternal(1, "XR_ERROR_LOCALIZATION_MAP_PERMISSION_DENIED_ML: The com.magicleap.permission.SPACE_MANAGER permission was denied. (Added by the XR_ML_localization_map extension) (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
#endif

#ifdef XR_ERROR_LOCALIZATION_MAP_ALREADY_EXISTS_ML
    case XR_ERROR_LOCALIZATION_MAP_ALREADY_EXISTS_ML:
        logMessageInternal(1, "XR_ERROR_LOCALIZATION_MAP_ALREADY_EXISTS_ML: The map being imported already exists in the system. (Added by the XR_ML_localization_map extension) (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
#endif

#ifdef XR_ERROR_LOCALIZATION_MAP_CANNOT_EXPORT_CLOUD_MAP_ML
    case XR_ERROR_LOCALIZATION_MAP_CANNOT_EXPORT_CLOUD_MAP_ML:
        logMessageInternal(1, "XR_ERROR_LOCALIZATION_MAP_CANNOT_EXPORT_CLOUD_MAP_ML: The map localization service cannot export cloud based maps. (Added by the XR_ML_localization_map extension) (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
#endif

#ifdef XR_ERROR_SPATIAL_ANCHORS_PERMISSION_DENIED_ML
    case XR_ERROR_SPATIAL_ANCHORS_PERMISSION_DENIED_ML:
        logMessageInternal(1, "XR_ERROR_SPATIAL_ANCHORS_PERMISSION_DENIED_ML: The com.magicleap.permission.SPATIAL_ANCHOR permission was not granted. (Added by the XR_ML_spatial_anchors extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_ML_spatial_anchors"}, file, line);
        break;
#endif

#ifdef XR_ERROR_SPATIAL_ANCHORS_NOT_LOCALIZED_ML
    case XR_ERROR_SPATIAL_ANCHORS_NOT_LOCALIZED_ML:
        logMessageInternal(1, "XR_ERROR_SPATIAL_ANCHORS_NOT_LOCALIZED_ML: Operation failed because the system is not localized into a localization map. (Added by the XR_ML_spatial_anchors extension) (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
#endif

#ifdef XR_ERROR_SPATIAL_ANCHORS_OUT_OF_MAP_BOUNDS_ML
    case XR_ERROR_SPATIAL_ANCHORS_OUT_OF_MAP_BOUNDS_ML:
        logMessageInternal(1, "XR_ERROR_SPATIAL_ANCHORS_OUT_OF_MAP_BOUNDS_ML: Operation failed because it is performed outside of the localization map. (Added by the XR_ML_spatial_anchors extension) (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
#endif

#ifdef XR_ERROR_SPATIAL_ANCHORS_SPACE_NOT_LOCATABLE_ML
    case XR_ERROR_SPATIAL_ANCHORS_SPACE_NOT_LOCATABLE_ML:
        logMessageInternal(1, "XR_ERROR_SPATIAL_ANCHORS_SPACE_NOT_LOCATABLE_ML: Operation failed because the space referenced cannot be located. (Added by the XR_ML_spatial_anchors extension) (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
#endif

#ifdef XR_ERROR_SPATIAL_ANCHORS_ANCHOR_NOT_FOUND_ML
    case XR_ERROR_SPATIAL_ANCHORS_ANCHOR_NOT_FOUND_ML:
        logMessageInternal(1, "XR_ERROR_SPATIAL_ANCHORS_ANCHOR_NOT_FOUND_ML: The anchor references was not found. (Added by the XR_ML_spatial_anchors_storage extension) (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
#endif

#ifdef XR_ERROR_SPATIAL_ANCHOR_NAME_NOT_FOUND_MSFT
    case XR_ERROR_SPATIAL_ANCHOR_NAME_NOT_FOUND_MSFT:
        logMessageInternal(1, "XR_ERROR_SPATIAL_ANCHOR_NAME_NOT_FOUND_MSFT: A spatial anchor was not found associated with the spatial anchor name provided. (Added by the XR_MSFT_spatial_anchor_persistence extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_MSFT_spatial_anchor_persistence"}, file, line);
        break;
#endif

#ifdef XR_ERROR_SPACE_MAPPING_INSUFFICIENT_FB
    case XR_ERROR_SPACE_MAPPING_INSUFFICIENT_FB:
        logMessageInternal(1, "XR_ERROR_SPACE_MAPPING_INSUFFICIENT_FB: Anchor import from cloud or export from device failed. (Added by the XR_FB_spatial_entity_sharing extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_FB_spatial_entity_sharing"}, file, line);
        break;
#endif

#ifdef XR_ERROR_SPACE_LOCALIZATION_FAILED_FB
    case XR_ERROR_SPACE_LOCALIZATION_FAILED_FB:
        logMessageInternal(1, "XR_ERROR_SPACE_LOCALIZATION_FAILED_FB: Anchors were downloaded from the cloud but failed to be imported/aligned on the device. (Added by the XR_FB_spatial_entity_sharing extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_FB_spatial_entity_sharing"}, file, line);
        break;
#endif

#ifdef XR_ERROR_SPACE_NETWORK_TIMEOUT_FB
    case XR_ERROR_SPACE_NETWORK_TIMEOUT_FB:
        logMessageInternal(1, "XR_ERROR_SPACE_NETWORK_TIMEOUT_FB: Timeout occurred while waiting for network request to complete. (Added by the XR_FB_spatial_entity_sharing extension) (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
#endif

#ifdef XR_ERROR_SPACE_NETWORK_REQUEST_FAILED_FB
    case XR_ERROR_SPACE_NETWORK_REQUEST_FAILED_FB:
        logMessageInternal(1, "XR_ERROR_SPACE_NETWORK_REQUEST_FAILED_FB: The network request failed. (Added by the XR_FB_spatial_entity_sharing extension) (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
#endif

#ifdef XR_ERROR_SPACE_CLOUD_STORAGE_DISABLED_FB
    case XR_ERROR_SPACE_CLOUD_STORAGE_DISABLED_FB:
        logMessageInternal(1, "XR_ERROR_SPACE_CLOUD_STORAGE_DISABLED_FB: Cloud storage is required for this operation but is currently disabled. (Added by the XR_FB_spatial_entity_sharing extension) (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
#endif

#ifdef XR_ERROR_SPACE_INSUFFICIENT_RESOURCES_META
    case XR_ERROR_SPACE_INSUFFICIENT_RESOURCES_META:
        logMessageInternal(1, "XR_ERROR_SPACE_INSUFFICIENT_RESOURCES_META: Resource limitation prevented this operation from executing. Recommend retrying later or reducing memory consumption. (Added by the XR_META_spatial_entity_persistence extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_META_spatial_entity_persistence"}, file, line);
        break;
#endif

#ifdef XR_ERROR_SPACE_STORAGE_AT_CAPACITY_META
    case XR_ERROR_SPACE_STORAGE_AT_CAPACITY_META:
        logMessageInternal(1, "XR_ERROR_SPACE_STORAGE_AT_CAPACITY_META: Operation could not be completed until resources used are reduced or storage expanded. (Added by the XR_META_spatial_entity_persistence extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_META_spatial_entity_persistence"}, file, line);
        break;
#endif

#ifdef XR_ERROR_SPACE_INSUFFICIENT_VIEW_META
    case XR_ERROR_SPACE_INSUFFICIENT_VIEW_META:
        logMessageInternal(1, "XR_ERROR_SPACE_INSUFFICIENT_VIEW_META: Look around the environment more for space tracking to function. (Added by the XR_META_spatial_entity_persistence extension) (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
#endif

#ifdef XR_ERROR_SPACE_PERMISSION_INSUFFICIENT_META
    case XR_ERROR_SPACE_PERMISSION_INSUFFICIENT_META:
        logMessageInternal(1, "XR_ERROR_SPACE_PERMISSION_INSUFFICIENT_META: Space operation permission insufficient. Confirm required permissions for using Space APIs. (Added by the XR_META_spatial_entity_persistence extension) (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
#endif

#ifdef XR_ERROR_SPACE_RATE_LIMITED_META
    case XR_ERROR_SPACE_RATE_LIMITED_META:
        logMessageInternal(1, "XR_ERROR_SPACE_RATE_LIMITED_META: Operation cancelled due to rate limiting. Recommend retrying after a short delay. (Added by the XR_META_spatial_entity_persistence extension) (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
#endif

#ifdef XR_ERROR_SPACE_TOO_DARK_META
    case XR_ERROR_SPACE_TOO_DARK_META:
        logMessageInternal(1, "XR_ERROR_SPACE_TOO_DARK_META: Environment too dark for tracking to complete operation. (Added by the XR_META_spatial_entity_persistence extension) (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
#endif

#ifdef XR_ERROR_SPACE_TOO_BRIGHT_META
    case XR_ERROR_SPACE_TOO_BRIGHT_META:
        logMessageInternal(1, "XR_ERROR_SPACE_TOO_BRIGHT_META: Environment too bright for tracking to complete operation. (Added by the XR_META_spatial_entity_persistence extension) (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
#endif

#ifdef XR_ERROR_PASSTHROUGH_COLOR_LUT_BUFFER_SIZE_MISMATCH_META
    case XR_ERROR_PASSTHROUGH_COLOR_LUT_BUFFER_SIZE_MISMATCH_META:
        logMessageInternal(1, "XR_ERROR_PASSTHROUGH_COLOR_LUT_BUFFER_SIZE_MISMATCH_META: The provided data buffer did not match the required size. (Added by the XR_META_passthrough_color_lut extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_META_passthrough_color_lut"}, file, line);
        break;
#endif

#ifdef XR_ERROR_RENDER_MODEL_ID_INVALID_EXT
    case XR_ERROR_RENDER_MODEL_ID_INVALID_EXT:
        logMessageInternal(1, "XR_ERROR_RENDER_MODEL_ID_INVALID_EXT: The render model ID is invalid. (Added by the XR_EXT_render_model extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_EXT_render_model"}, file, line);
        break;
#endif

#ifdef XR_ERROR_RENDER_MODEL_ASSET_UNAVAILABLE_EXT
    case XR_ERROR_RENDER_MODEL_ASSET_UNAVAILABLE_EXT:
        logMessageInternal(1, "XR_ERROR_RENDER_MODEL_ASSET_UNAVAILABLE_EXT: The render model asset is unavailable. (Added by the XR_EXT_render_model extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_EXT_render_model"}, file, line);
        break;
#endif

#ifdef XR_ERROR_RENDER_MODEL_GLTF_EXTENSION_REQUIRED_EXT
    case XR_ERROR_RENDER_MODEL_GLTF_EXTENSION_REQUIRED_EXT:
        logMessageInternal(1, "XR_ERROR_RENDER_MODEL_GLTF_EXTENSION_REQUIRED_EXT: A glTF extension is required. (Added by the XR_EXT_render_model extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_EXT_render_model"}, file, line);
        break;
#endif

#ifdef XR_ERROR_NOT_INTERACTION_RENDER_MODEL_EXT
    case XR_ERROR_NOT_INTERACTION_RENDER_MODEL_EXT:
        logMessageInternal(1, "XR_ERROR_NOT_INTERACTION_RENDER_MODEL_EXT: The provided XrRenderModelEXT was not created from a XrRenderModelIdEXT. (Added by the XR_EXT_interaction_render_model extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_EXT_interaction_render_model"}, file, line);
        break;
#endif

#ifdef XR_ERROR_HINT_ALREADY_SET_QCOM
    case XR_ERROR_HINT_ALREADY_SET_QCOM:
        logMessageInternal(1, "XR_ERROR_HINT_ALREADY_SET_QCOM: Tracking optimization hint is already set for the domain. (Added by the XR_QCOM_tracking_optimization_settings extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_QCOM_tracking_optimization_settings"}, file, line);
        break;
#endif

#ifdef XR_ERROR_NOT_AN_ANCHOR_HTC
    case XR_ERROR_NOT_AN_ANCHOR_HTC:
        logMessageInternal(1, "XR_ERROR_NOT_AN_ANCHOR_HTC: The provided space is valid but not an anchor. (Added by the XR_HTC_anchor extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_HTC_anchor"}, file, line);
        break;
#endif

#ifdef XR_ERROR_SPATIAL_ENTITY_ID_INVALID_BD
    case XR_ERROR_SPATIAL_ENTITY_ID_INVALID_BD:
        logMessageInternal(1, "XR_ERROR_SPATIAL_ENTITY_ID_INVALID_BD: The spatial entity id is invalid. (Added by the XR_BD_spatial_sensing extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_BD_spatial_sensing"}, file, line);
        break;
#endif

#ifdef XR_ERROR_SPATIAL_SENSING_SERVICE_UNAVAILABLE_BD
    case XR_ERROR_SPATIAL_SENSING_SERVICE_UNAVAILABLE_BD:
        logMessageInternal(1, "XR_ERROR_SPATIAL_SENSING_SERVICE_UNAVAILABLE_BD: The spatial sensing service is unavailable. (Added by the XR_BD_spatial_sensing extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_BD_spatial_sensing"}, file, line);
        break;
#endif

#ifdef XR_ERROR_ANCHOR_NOT_SUPPORTED_FOR_ENTITY_BD
    case XR_ERROR_ANCHOR_NOT_SUPPORTED_FOR_ENTITY_BD:
        logMessageInternal(1, "XR_ERROR_ANCHOR_NOT_SUPPORTED_FOR_ENTITY_BD: The spatial entity does not support anchor. (Added by the XR_BD_spatial_sensing extension) (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
#endif

#ifdef XR_ERROR_SPATIAL_ANCHOR_NOT_FOUND_BD
    case XR_ERROR_SPATIAL_ANCHOR_NOT_FOUND_BD:
        logMessageInternal(1, "XR_ERROR_SPATIAL_ANCHOR_NOT_FOUND_BD: The spatial anchor is not found. (Added by the XR_BD_spatial_anchor extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_BD_spatial_anchor"}, file, line);
        break;
#endif

#ifdef XR_ERROR_SPATIAL_ANCHOR_SHARING_NETWORK_TIMEOUT_BD
    case XR_ERROR_SPATIAL_ANCHOR_SHARING_NETWORK_TIMEOUT_BD:
        logMessageInternal(1, "XR_ERROR_SPATIAL_ANCHOR_SHARING_NETWORK_TIMEOUT_BD: The network transmission timeout. (Added by the XR_BD_spatial_anchor_sharing extension) (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
#endif

#ifdef XR_ERROR_SPATIAL_ANCHOR_SHARING_AUTHENTICATION_FAILURE_BD
    case XR_ERROR_SPATIAL_ANCHOR_SHARING_AUTHENTICATION_FAILURE_BD:
        logMessageInternal(1, "XR_ERROR_SPATIAL_ANCHOR_SHARING_AUTHENTICATION_FAILURE_BD: The authentication for the user account failed. (Added by the XR_BD_spatial_anchor_sharing extension) (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
#endif

#ifdef XR_ERROR_SPATIAL_ANCHOR_SHARING_NETWORK_FAILURE_BD
    case XR_ERROR_SPATIAL_ANCHOR_SHARING_NETWORK_FAILURE_BD:
        logMessageInternal(1, "XR_ERROR_SPATIAL_ANCHOR_SHARING_NETWORK_FAILURE_BD: The network connection failed. (Added by the XR_BD_spatial_anchor_sharing extension) (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
#endif

#ifdef XR_ERROR_SPATIAL_ANCHOR_SHARING_LOCALIZATION_FAIL_BD
    case XR_ERROR_SPATIAL_ANCHOR_SHARING_LOCALIZATION_FAIL_BD:
        logMessageInternal(1, "XR_ERROR_SPATIAL_ANCHOR_SHARING_LOCALIZATION_FAIL_BD: The spatial anchor localization failed. (Added by the XR_BD_spatial_anchor_sharing extension) (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
#endif

#ifdef XR_ERROR_SPATIAL_ANCHOR_SHARING_MAP_INSUFFICIENT_BD
    case XR_ERROR_SPATIAL_ANCHOR_SHARING_MAP_INSUFFICIENT_BD:
        logMessageInternal(1, "XR_ERROR_SPATIAL_ANCHOR_SHARING_MAP_INSUFFICIENT_BD: The feature points of spatial anchor map are insufficient. (Added by the XR_BD_spatial_anchor_sharing extension) (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
#endif

#ifdef XR_ERROR_SCENE_CAPTURE_FAILURE_BD
    case XR_ERROR_SCENE_CAPTURE_FAILURE_BD:
        logMessageInternal(1, "XR_ERROR_SCENE_CAPTURE_FAILURE_BD: The scene capture is failed, for example exiting abnormally. (Added by the XR_BD_spatial_scene extension) (" + std::to_string(result) + ")", {"OpenXR"}, file, line);
        break;
#endif

#ifdef XR_ERROR_SPACE_NOT_LOCATABLE_EXT
    case XR_ERROR_SPACE_NOT_LOCATABLE_EXT:
        logMessageInternal(1, "XR_ERROR_SPACE_NOT_LOCATABLE_EXT: The space passed to the function was not locatable. (Added by the XR_EXT_plane_detection extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_EXT_plane_detection"}, file, line);
        break;
#endif

#ifdef XR_ERROR_PLANE_DETECTION_PERMISSION_DENIED_EXT
    case XR_ERROR_PLANE_DETECTION_PERMISSION_DENIED_EXT:
        logMessageInternal(1, "XR_ERROR_PLANE_DETECTION_PERMISSION_DENIED_EXT: The permission for this resource was not granted. (Added by the XR_EXT_plane_detection extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_EXT_plane_detection"}, file, line);
        break;
#endif

#ifdef XR_ERROR_MISMATCHING_TRACKABLE_TYPE_ANDROID
    case XR_ERROR_MISMATCHING_TRACKABLE_TYPE_ANDROID:
        logMessageInternal(1, "XR_ERROR_MISMATCHING_TRACKABLE_TYPE_ANDROID: Indicates that the parameters contains multiple trackable types. (Added by the XR_ANDROID_trackables extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_ANDROID_trackables"}, file, line);
        break;
#endif

#ifdef XR_ERROR_TRACKABLE_TYPE_NOT_SUPPORTED_ANDROID
    case XR_ERROR_TRACKABLE_TYPE_NOT_SUPPORTED_ANDROID:
        logMessageInternal(1, "XR_ERROR_TRACKABLE_TYPE_NOT_SUPPORTED_ANDROID: Function not supported by the given trackable type. (Added by the XR_ANDROID_trackables extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_ANDROID_trackables"}, file, line);
        break;
#endif

#ifdef XR_ERROR_ANCHOR_ID_NOT_FOUND_ANDROID
    case XR_ERROR_ANCHOR_ID_NOT_FOUND_ANDROID:
        logMessageInternal(1, "XR_ERROR_ANCHOR_ID_NOT_FOUND_ANDROID: XrUuidExt passed to the function was not found to be a persisted anchor. (Added by the XR_ANDROID_device_anchor_persistence extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_ANDROID_device_anchor_persistence"}, file, line);
        break;
#endif

#ifdef XR_ERROR_ANCHOR_ALREADY_PERSISTED_ANDROID
    case XR_ERROR_ANCHOR_ALREADY_PERSISTED_ANDROID:
        logMessageInternal(1, "XR_ERROR_ANCHOR_ALREADY_PERSISTED_ANDROID: XrUuidExt passed to the function was already marked to be persisted. (Added by the XR_ANDROID_device_anchor_persistence extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_ANDROID_device_anchor_persistence"}, file, line);
        break;
#endif

#ifdef XR_ERROR_ANCHOR_NOT_TRACKING_ANDROID
    case XR_ERROR_ANCHOR_NOT_TRACKING_ANDROID:
        logMessageInternal(1, "XR_ERROR_ANCHOR_NOT_TRACKING_ANDROID: Anchor cannot be persisted because it is not tracking. (Added by the XR_ANDROID_device_anchor_persistence extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_ANDROID_device_anchor_persistence"}, file, line);
        break;
#endif

#ifdef XR_ERROR_PERSISTED_DATA_NOT_READY_ANDROID
    case XR_ERROR_PERSISTED_DATA_NOT_READY_ANDROID:
        logMessageInternal(1, "XR_ERROR_PERSISTED_DATA_NOT_READY_ANDROID: Persisted data stored by this app has not been loaded yet. (Added by the XR_ANDROID_device_anchor_persistence extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_ANDROID_device_anchor_persistence"}, file, line);
        break;
#endif

#ifdef XR_ERROR_SERVICE_NOT_READY_ANDROID
    case XR_ERROR_SERVICE_NOT_READY_ANDROID:
        logMessageInternal(1, "XR_ERROR_SERVICE_NOT_READY_ANDROID: The underlying tracking service is not yet ready. (Added by the XR_ANDROID_face_tracking extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_ANDROID_face_tracking"}, file, line);
        break;
#endif

#ifdef XR_ERROR_FUTURE_PENDING_EXT
    case XR_ERROR_FUTURE_PENDING_EXT:
        logMessageInternal(1, "XR_ERROR_FUTURE_PENDING_EXT: Returned by completion function to indicate future is not ready. (Added by the XR_EXT_future extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_EXT_future"}, file, line);
        break;
#endif

#ifdef XR_ERROR_FUTURE_INVALID_EXT
    case XR_ERROR_FUTURE_INVALID_EXT:
        logMessageInternal(1, "XR_ERROR_FUTURE_INVALID_EXT: Returned by completion function to indicate future is not valid. (Added by the XR_EXT_future extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_EXT_future"}, file, line);
        break;
#endif

#ifdef XR_ERROR_SYSTEM_NOTIFICATION_PERMISSION_DENIED_ML
    case XR_ERROR_SYSTEM_NOTIFICATION_PERMISSION_DENIED_ML:
        logMessageInternal(1, "XR_ERROR_SYSTEM_NOTIFICATION_PERMISSION_DENIED_ML: The com.magicleap.permission.SYSTEM_NOTIFICATION permission was not granted. (Added by the XR_ML_system_notifications extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_ML_system_notifications"}, file, line);
        break;
#endif

#ifdef XR_ERROR_SYSTEM_NOTIFICATION_INCOMPATIBLE_SKU_ML
    case XR_ERROR_SYSTEM_NOTIFICATION_INCOMPATIBLE_SKU_ML:
        logMessageInternal(1, "XR_ERROR_SYSTEM_NOTIFICATION_INCOMPATIBLE_SKU_ML: Incompatible SKU detected. (Added by the XR_ML_system_notifications extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_ML_system_notifications"}, file, line);
        break;
#endif

#ifdef XR_ERROR_WORLD_MESH_DETECTOR_PERMISSION_DENIED_ML
    case XR_ERROR_WORLD_MESH_DETECTOR_PERMISSION_DENIED_ML:
        logMessageInternal(1, "XR_ERROR_WORLD_MESH_DETECTOR_PERMISSION_DENIED_ML: The world mesh detector permission was not granted. (Added by the XR_ML_world_mesh_detection extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_ML_world_mesh_detection"}, file, line);
        break;
#endif

#ifdef XR_ERROR_WORLD_MESH_DETECTOR_SPACE_NOT_LOCATABLE_ML
    case XR_ERROR_WORLD_MESH_DETECTOR_SPACE_NOT_LOCATABLE_ML:
        logMessageInternal(1, "XR_ERROR_WORLD_MESH_DETECTOR_SPACE_NOT_LOCATABLE_ML: The runtime was unable to locate the space and cannot fulfill your request. (Added by the XR_ML_world_mesh_detection extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_ML_world_mesh_detection"}, file, line);
        break;
#endif

#ifdef XR_ERROR_COLOCATION_DISCOVERY_NETWORK_FAILED_META
    case XR_ERROR_COLOCATION_DISCOVERY_NETWORK_FAILED_META:
        logMessageInternal(1, "XR_ERROR_COLOCATION_DISCOVERY_NETWORK_FAILED_META: The network request failed. (Added by the XR_META_colocation_discovery extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_META_colocation_discovery"}, file, line);
        break;
#endif

#ifdef XR_ERROR_COLOCATION_DISCOVERY_NO_DISCOVERY_METHOD_META
    case XR_ERROR_COLOCATION_DISCOVERY_NO_DISCOVERY_METHOD_META:
        logMessageInternal(1, "XR_ERROR_COLOCATION_DISCOVERY_NO_DISCOVERY_METHOD_META: The runtime does not have any methods available to perform discovery. (Added by the XR_META_colocation_discovery extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_META_colocation_discovery"}, file, line);
        break;
#endif

#ifdef XR_ERROR_SPACE_GROUP_NOT_FOUND_META
    case XR_ERROR_SPACE_GROUP_NOT_FOUND_META:
        logMessageInternal(1, "XR_ERROR_SPACE_GROUP_NOT_FOUND_META: The group UUID was not found within the runtime. (Added by the XR_META_spatial_entity_group_sharing extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_META_spatial_entity_group_sharing"}, file, line);
        break;
#endif

#ifdef XR_ERROR_ANCHOR_NOT_OWNED_BY_CALLER_ANDROID
    case XR_ERROR_ANCHOR_NOT_OWNED_BY_CALLER_ANDROID:
        logMessageInternal(1, "XR_ERROR_ANCHOR_NOT_OWNED_BY_CALLER_ANDROID: Anchor not owned by XrSession in which function is called. (Added by the XR_ANDROID_anchor_sharing_export extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_ANDROID_anchor_sharing_export"}, file, line);
        break;
#endif

#ifdef XR_ERROR_EXTENSION_DEPENDENCY_NOT_ENABLED_KHR
    case XR_ERROR_EXTENSION_DEPENDENCY_NOT_ENABLED_KHR:
        logMessageInternal(1, "XR_ERROR_EXTENSION_DEPENDENCY_NOT_ENABLED_KHR: Alias for XR_ERROR_EXTENSION_DEPENDENCY_NOT_ENABLED (Added by the XR_KHR_maintenance1 extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_KHR_maintenance1"}, file, line);
        break;
#endif

#ifdef XR_ERROR_PERMISSION_INSUFFICIENT_KHR
    case XR_ERROR_PERMISSION_INSUFFICIENT_KHR:
        logMessageInternal(1, "XR_ERROR_PERMISSION_INSUFFICIENT_KHR: Alias for XR_ERROR_PERMISSION_INSUFFICIENT (Added by the XR_KHR_maintenance1 extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_KHR_maintenance1"}, file, line);
        break;
#endif

#ifdef XR_ERROR_SPATIAL_CAPABILITY_UNSUPPORTED_EXT
    case XR_ERROR_SPATIAL_CAPABILITY_UNSUPPORTED_EXT:
        logMessageInternal(1, "XR_ERROR_SPATIAL_CAPABILITY_UNSUPPORTED_EXT: The specified spatial capability is not supported by the runtime or system. (Added by the XR_EXT_spatial_entity extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_EXT_spatial_entity"}, file, line);
        break;
#endif

#ifdef XR_ERROR_SPATIAL_ENTITY_ID_INVALID_EXT
    case XR_ERROR_SPATIAL_ENTITY_ID_INVALID_EXT:
        logMessageInternal(1, "XR_ERROR_SPATIAL_ENTITY_ID_INVALID_EXT: The specified spatial entity id is invalid or does not exist. (Added by the XR_EXT_spatial_entity extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_EXT_spatial_entity"}, file, line);
        break;
#endif

#ifdef XR_ERROR_SPATIAL_BUFFER_ID_INVALID_EXT
    case XR_ERROR_SPATIAL_BUFFER_ID_INVALID_EXT:
        logMessageInternal(1, "XR_ERROR_SPATIAL_BUFFER_ID_INVALID_EXT: The specified spatial buffer id is invalid or does not exist. (Added by the XR_EXT_spatial_entity extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_EXT_spatial_entity"}, file, line);
        break;
#endif

#ifdef XR_ERROR_SPATIAL_COMPONENT_UNSUPPORTED_FOR_CAPABILITY_EXT
    case XR_ERROR_SPATIAL_COMPONENT_UNSUPPORTED_FOR_CAPABILITY_EXT:
        logMessageInternal(1, "XR_ERROR_SPATIAL_COMPONENT_UNSUPPORTED_FOR_CAPABILITY_EXT: The specified spatial component is not supported for the given capability. (Added by the XR_EXT_spatial_entity extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_EXT_spatial_entity"}, file, line);
        break;
#endif

#ifdef XR_ERROR_SPATIAL_CAPABILITY_CONFIGURATION_INVALID_EXT
    case XR_ERROR_SPATIAL_CAPABILITY_CONFIGURATION_INVALID_EXT:
        logMessageInternal(1, "XR_ERROR_SPATIAL_CAPABILITY_CONFIGURATION_INVALID_EXT: The specified spatial capability configuration is invalid. (Added by the XR_EXT_spatial_entity extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_EXT_spatial_entity"}, file, line);
        break;
#endif

#ifdef XR_ERROR_SPATIAL_COMPONENT_NOT_ENABLED_EXT
    case XR_ERROR_SPATIAL_COMPONENT_NOT_ENABLED_EXT:
        logMessageInternal(1, "XR_ERROR_SPATIAL_COMPONENT_NOT_ENABLED_EXT: The specified spatial component is not enabled for the spatial context. (Added by the XR_EXT_spatial_entity extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_EXT_spatial_entity"}, file, line);
        break;
#endif

#ifdef XR_ERROR_SPATIAL_PERSISTENCE_SCOPE_UNSUPPORTED_EXT
    case XR_ERROR_SPATIAL_PERSISTENCE_SCOPE_UNSUPPORTED_EXT:
        logMessageInternal(1, "XR_ERROR_SPATIAL_PERSISTENCE_SCOPE_UNSUPPORTED_EXT: The specified spatial persistence scope is not supported. (Added by the XR_EXT_spatial_persistence extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_EXT_spatial_persistence"}, file, line);
        break;
#endif

#ifdef XR_ERROR_SPATIAL_PERSISTENCE_SCOPE_INCOMPATIBLE_EXT
    case XR_ERROR_SPATIAL_PERSISTENCE_SCOPE_INCOMPATIBLE_EXT:
        logMessageInternal(1, "XR_ERROR_SPATIAL_PERSISTENCE_SCOPE_INCOMPATIBLE_EXT: The configured scope is incompatible for the current spatial entity. (Added by the XR_EXT_spatial_persistence_operations extension) (" + std::to_string(result) + ")", {"OpenXR", "XR_EXT_spatial_persistence_operations"}, file, line);
        break;
#endif

    default:
        logMessageInternal(1, "Unknown XrResult error code: " + std::to_string(result), {"OpenXR"}, file, line);
        break;
    }
}
