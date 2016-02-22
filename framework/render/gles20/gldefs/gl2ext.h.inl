#define EXTNAME "GL_KHR_blend_equation_advanced"
GLAPIPROCEXT(void, BlendBarrierKHR, (void), PFNGLBLENDBARRIERKHRPROC, EXTNAME)

#undef EXTNAME /* GL_KHR_blend_equation_advanced */
#define EXTNAME "GL_KHR_debug"
GLAPIPROCEXT(void, DebugMessageControlKHR, (GLenum source, GLenum type, GLenum severity, GLsizei count, const GLuint *ids, GLboolean enabled), PFNGLDEBUGMESSAGECONTROLKHRPROC, EXTNAME)
GLAPIPROCEXT(void, DebugMessageInsertKHR, (GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *buf), PFNGLDEBUGMESSAGEINSERTKHRPROC, EXTNAME)
GLAPIPROCEXT(void, DebugMessageCallbackKHR, (GLDEBUGPROCKHR callback, const void *userParam), PFNGLDEBUGMESSAGECALLBACKKHRPROC, EXTNAME)
GLAPIFUNCEXT(GLuint, GetDebugMessageLogKHR, (GLuint count, GLsizei bufSize, GLenum *sources, GLenum *types, GLuint *ids, GLenum *severities, GLsizei *lengths, GLchar *messageLog), PFNGLGETDEBUGMESSAGELOGKHRPROC, EXTNAME)
GLAPIPROCEXT(void, PushDebugGroupKHR, (GLenum source, GLuint id, GLsizei length, const GLchar *message), PFNGLPUSHDEBUGGROUPKHRPROC, EXTNAME)
GLAPIPROCEXT(void, PopDebugGroupKHR, (void), PFNGLPOPDEBUGGROUPKHRPROC, EXTNAME)
GLAPIPROCEXT(void, ObjectLabelKHR, (GLenum identifier, GLuint name, GLsizei length, const GLchar *label), PFNGLOBJECTLABELKHRPROC, EXTNAME)
GLAPIPROCEXT(void, GetObjectLabelKHR, (GLenum identifier, GLuint name, GLsizei bufSize, GLsizei *length, GLchar *label), PFNGLGETOBJECTLABELKHRPROC, EXTNAME)
GLAPIPROCEXT(void, ObjectPtrLabelKHR, (const void *ptr, GLsizei length, const GLchar *label), PFNGLOBJECTPTRLABELKHRPROC, EXTNAME)
GLAPIPROCEXT(void, GetObjectPtrLabelKHR, (const void *ptr, GLsizei bufSize, GLsizei *length, GLchar *label), PFNGLGETOBJECTPTRLABELKHRPROC, EXTNAME)
GLAPIPROCEXT(void, GetPointervKHR, (GLenum pname, void **params), PFNGLGETPOINTERVKHRPROC, EXTNAME)

#undef EXTNAME /* GL_KHR_debug */
#define EXTNAME "GL_KHR_robustness"
GLAPIFUNCEXT(GLenum, GetGraphicsResetStatusKHR, (void), PFNGLGETGRAPHICSRESETSTATUSKHRPROC, EXTNAME)
GLAPIPROCEXT(void, ReadnPixelsKHR, (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLsizei bufSize, void *data), PFNGLREADNPIXELSKHRPROC, EXTNAME)
GLAPIPROCEXT(void, GetnUniformfvKHR, (GLuint program, GLint location, GLsizei bufSize, GLfloat *params), PFNGLGETNUNIFORMFVKHRPROC, EXTNAME)
GLAPIPROCEXT(void, GetnUniformivKHR, (GLuint program, GLint location, GLsizei bufSize, GLint *params), PFNGLGETNUNIFORMIVKHRPROC, EXTNAME)
GLAPIPROCEXT(void, GetnUniformuivKHR, (GLuint program, GLint location, GLsizei bufSize, GLuint *params), PFNGLGETNUNIFORMUIVKHRPROC, EXTNAME)

#undef EXTNAME /* GL_KHR_robustness */
#define EXTNAME "GL_OES_EGL_image"
GLAPIPROCEXT(void, EGLImageTargetTexture2DOES, (GLenum target, GLeglImageOES image), PFNGLEGLIMAGETARGETTEXTURE2DOESPROC, EXTNAME)
GLAPIPROCEXT(void, EGLImageTargetRenderbufferStorageOES, (GLenum target, GLeglImageOES image), PFNGLEGLIMAGETARGETRENDERBUFFERSTORAGEOESPROC, EXTNAME)

#undef EXTNAME /* GL_OES_EGL_image */
#define EXTNAME "GL_OES_copy_image"
GLAPIPROCEXT(void, CopyImageSubDataOES, (GLuint srcName, GLenum srcTarget, GLint srcLevel, GLint srcX, GLint srcY, GLint srcZ, GLuint dstName, GLenum dstTarget, GLint dstLevel, GLint dstX, GLint dstY, GLint dstZ, GLsizei srcWidth, GLsizei srcHeight, GLsizei srcDepth), PFNGLCOPYIMAGESUBDATAOESPROC, EXTNAME)

#undef EXTNAME /* GL_OES_copy_image */
#define EXTNAME "GL_OES_draw_buffers_indexed"
GLAPIPROCEXT(void, EnableiOES, (GLenum target, GLuint index), PFNGLENABLEIOESPROC, EXTNAME)
GLAPIPROCEXT(void, DisableiOES, (GLenum target, GLuint index), PFNGLDISABLEIOESPROC, EXTNAME)
GLAPIPROCEXT(void, BlendEquationiOES, (GLuint buf, GLenum mode), PFNGLBLENDEQUATIONIOESPROC, EXTNAME)
GLAPIPROCEXT(void, BlendEquationSeparateiOES, (GLuint buf, GLenum modeRGB, GLenum modeAlpha), PFNGLBLENDEQUATIONSEPARATEIOESPROC, EXTNAME)
GLAPIPROCEXT(void, BlendFunciOES, (GLuint buf, GLenum src, GLenum dst), PFNGLBLENDFUNCIOESPROC, EXTNAME)
GLAPIPROCEXT(void, BlendFuncSeparateiOES, (GLuint buf, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha), PFNGLBLENDFUNCSEPARATEIOESPROC, EXTNAME)
GLAPIPROCEXT(void, ColorMaskiOES, (GLuint index, GLboolean r, GLboolean g, GLboolean b, GLboolean a), PFNGLCOLORMASKIOESPROC, EXTNAME)
GLAPIFUNCEXT(GLboolean, IsEnablediOES, (GLenum target, GLuint index), PFNGLISENABLEDIOESPROC, EXTNAME)

#undef EXTNAME /* GL_OES_draw_buffers_indexed */
#define EXTNAME "GL_OES_draw_elements_base_vertex"
GLAPIPROCEXT(void, DrawElementsBaseVertexOES, (GLenum mode, GLsizei count, GLenum type, const void *indices, GLint basevertex), PFNGLDRAWELEMENTSBASEVERTEXOESPROC, EXTNAME)
GLAPIPROCEXT(void, DrawRangeElementsBaseVertexOES, (GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void *indices, GLint basevertex), PFNGLDRAWRANGEELEMENTSBASEVERTEXOESPROC, EXTNAME)
GLAPIPROCEXT(void, DrawElementsInstancedBaseVertexOES, (GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount, GLint basevertex), PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXOESPROC, EXTNAME)
GLAPIPROCEXT(void, MultiDrawElementsBaseVertexOES, (GLenum mode, const GLsizei *count, GLenum type, const void *const*indices, GLsizei primcount, const GLint *basevertex), PFNGLMULTIDRAWELEMENTSBASEVERTEXOESPROC, EXTNAME)

#undef EXTNAME /* GL_OES_draw_elements_base_vertex */
#define EXTNAME "GL_OES_geometry_shader"
GLAPIPROCEXT(void, FramebufferTextureOES, (GLenum target, GLenum attachment, GLuint texture, GLint level), PFNGLFRAMEBUFFERTEXTUREOESPROC, EXTNAME)

#undef EXTNAME /* GL_OES_geometry_shader */
#define EXTNAME "GL_OES_get_program_binary"
GLAPIPROCEXT(void, GetProgramBinaryOES, (GLuint program, GLsizei bufSize, GLsizei *length, GLenum *binaryFormat, void *binary), PFNGLGETPROGRAMBINARYOESPROC, EXTNAME)
GLAPIPROCEXT(void, ProgramBinaryOES, (GLuint program, GLenum binaryFormat, const void *binary, GLint length), PFNGLPROGRAMBINARYOESPROC, EXTNAME)

#undef EXTNAME /* GL_OES_get_program_binary */
#define EXTNAME "GL_OES_mapbuffer"
GLAPIFUNCEXT(void *, MapBufferOES, (GLenum target, GLenum access), PFNGLMAPBUFFEROESPROC, EXTNAME)
GLAPIFUNCEXT(GLboolean, UnmapBufferOES, (GLenum target), PFNGLUNMAPBUFFEROESPROC, EXTNAME)
GLAPIPROCEXT(void, GetBufferPointervOES, (GLenum target, GLenum pname, void **params), PFNGLGETBUFFERPOINTERVOESPROC, EXTNAME)

#undef EXTNAME /* GL_OES_mapbuffer */
#define EXTNAME "GL_OES_primitive_bounding_box"
GLAPIPROCEXT(void, PrimitiveBoundingBoxOES, (GLfloat minX, GLfloat minY, GLfloat minZ, GLfloat minW, GLfloat maxX, GLfloat maxY, GLfloat maxZ, GLfloat maxW), PFNGLPRIMITIVEBOUNDINGBOXOESPROC, EXTNAME)

#undef EXTNAME /* GL_OES_primitive_bounding_box */
#define EXTNAME "GL_OES_sample_shading"
GLAPIPROCEXT(void, MinSampleShadingOES, (GLfloat value), PFNGLMINSAMPLESHADINGOESPROC, EXTNAME)

#undef EXTNAME /* GL_OES_sample_shading */
#define EXTNAME "GL_OES_tessellation_shader"
GLAPIPROCEXT(void, PatchParameteriOES, (GLenum pname, GLint value), PFNGLPATCHPARAMETERIOESPROC, EXTNAME)

#undef EXTNAME /* GL_OES_tessellation_shader */
#define EXTNAME "GL_OES_texture_3D"
GLAPIPROCEXT(void, TexImage3DOES, (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void *pixels), PFNGLTEXIMAGE3DOESPROC, EXTNAME)
GLAPIPROCEXT(void, TexSubImage3DOES, (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void *pixels), PFNGLTEXSUBIMAGE3DOESPROC, EXTNAME)
GLAPIPROCEXT(void, CopyTexSubImage3DOES, (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height), PFNGLCOPYTEXSUBIMAGE3DOESPROC, EXTNAME)
GLAPIPROCEXT(void, CompressedTexImage3DOES, (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void *data), PFNGLCOMPRESSEDTEXIMAGE3DOESPROC, EXTNAME)
GLAPIPROCEXT(void, CompressedTexSubImage3DOES, (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void *data), PFNGLCOMPRESSEDTEXSUBIMAGE3DOESPROC, EXTNAME)
GLAPIPROCEXT(void, FramebufferTexture3DOES, (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint zoffset), PFNGLFRAMEBUFFERTEXTURE3DOESPROC, EXTNAME)

#undef EXTNAME /* GL_OES_texture_3D */
#define EXTNAME "GL_OES_texture_border_clamp"
GLAPIPROCEXT(void, TexParameterIivOES, (GLenum target, GLenum pname, const GLint *params), PFNGLTEXPARAMETERIIVOESPROC, EXTNAME)
GLAPIPROCEXT(void, TexParameterIuivOES, (GLenum target, GLenum pname, const GLuint *params), PFNGLTEXPARAMETERIUIVOESPROC, EXTNAME)
GLAPIPROCEXT(void, GetTexParameterIivOES, (GLenum target, GLenum pname, GLint *params), PFNGLGETTEXPARAMETERIIVOESPROC, EXTNAME)
GLAPIPROCEXT(void, GetTexParameterIuivOES, (GLenum target, GLenum pname, GLuint *params), PFNGLGETTEXPARAMETERIUIVOESPROC, EXTNAME)
GLAPIPROCEXT(void, SamplerParameterIivOES, (GLuint sampler, GLenum pname, const GLint *param), PFNGLSAMPLERPARAMETERIIVOESPROC, EXTNAME)
GLAPIPROCEXT(void, SamplerParameterIuivOES, (GLuint sampler, GLenum pname, const GLuint *param), PFNGLSAMPLERPARAMETERIUIVOESPROC, EXTNAME)
GLAPIPROCEXT(void, GetSamplerParameterIivOES, (GLuint sampler, GLenum pname, GLint *params), PFNGLGETSAMPLERPARAMETERIIVOESPROC, EXTNAME)
GLAPIPROCEXT(void, GetSamplerParameterIuivOES, (GLuint sampler, GLenum pname, GLuint *params), PFNGLGETSAMPLERPARAMETERIUIVOESPROC, EXTNAME)

#undef EXTNAME /* GL_OES_texture_border_clamp */
#define EXTNAME "GL_OES_texture_buffer"
GLAPIPROCEXT(void, TexBufferOES, (GLenum target, GLenum internalformat, GLuint buffer), PFNGLTEXBUFFEROESPROC, EXTNAME)
GLAPIPROCEXT(void, TexBufferRangeOES, (GLenum target, GLenum internalformat, GLuint buffer, GLintptr offset, GLsizeiptr size), PFNGLTEXBUFFERRANGEOESPROC, EXTNAME)

#undef EXTNAME /* GL_OES_texture_buffer */
#define EXTNAME "GL_OES_texture_storage_multisample_2d_array"
GLAPIPROCEXT(void, TexStorage3DMultisampleOES, (GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations), PFNGLTEXSTORAGE3DMULTISAMPLEOESPROC, EXTNAME)

#undef EXTNAME /* GL_OES_texture_storage_multisample_2d_array */
#define EXTNAME "GL_OES_texture_view"
GLAPIPROCEXT(void, TextureViewOES, (GLuint texture, GLenum target, GLuint origtexture, GLenum internalformat, GLuint minlevel, GLuint numlevels, GLuint minlayer, GLuint numlayers), PFNGLTEXTUREVIEWOESPROC, EXTNAME)

#undef EXTNAME /* GL_OES_texture_view */
#define EXTNAME "GL_OES_vertex_array_object"
GLAPIPROCEXT(void, BindVertexArrayOES, (GLuint array), PFNGLBINDVERTEXARRAYOESPROC, EXTNAME)
GLAPIPROCEXT(void, DeleteVertexArraysOES, (GLsizei n, const GLuint *arrays), PFNGLDELETEVERTEXARRAYSOESPROC, EXTNAME)
GLAPIPROCEXT(void, GenVertexArraysOES, (GLsizei n, GLuint *arrays), PFNGLGENVERTEXARRAYSOESPROC, EXTNAME)
GLAPIFUNCEXT(GLboolean, IsVertexArrayOES, (GLuint array), PFNGLISVERTEXARRAYOESPROC, EXTNAME)

#undef EXTNAME /* GL_OES_vertex_array_object */
#define EXTNAME "GL_AMD_performance_monitor"
GLAPIPROCEXT(void, GetPerfMonitorGroupsAMD, (GLint *numGroups, GLsizei groupsSize, GLuint *groups), PFNGLGETPERFMONITORGROUPSAMDPROC, EXTNAME)
GLAPIPROCEXT(void, GetPerfMonitorCountersAMD, (GLuint group, GLint *numCounters, GLint *maxActiveCounters, GLsizei counterSize, GLuint *counters), PFNGLGETPERFMONITORCOUNTERSAMDPROC, EXTNAME)
GLAPIPROCEXT(void, GetPerfMonitorGroupStringAMD, (GLuint group, GLsizei bufSize, GLsizei *length, GLchar *groupString), PFNGLGETPERFMONITORGROUPSTRINGAMDPROC, EXTNAME)
GLAPIPROCEXT(void, GetPerfMonitorCounterStringAMD, (GLuint group, GLuint counter, GLsizei bufSize, GLsizei *length, GLchar *counterString), PFNGLGETPERFMONITORCOUNTERSTRINGAMDPROC, EXTNAME)
GLAPIPROCEXT(void, GetPerfMonitorCounterInfoAMD, (GLuint group, GLuint counter, GLenum pname, void *data), PFNGLGETPERFMONITORCOUNTERINFOAMDPROC, EXTNAME)
GLAPIPROCEXT(void, GenPerfMonitorsAMD, (GLsizei n, GLuint *monitors), PFNGLGENPERFMONITORSAMDPROC, EXTNAME)
GLAPIPROCEXT(void, DeletePerfMonitorsAMD, (GLsizei n, GLuint *monitors), PFNGLDELETEPERFMONITORSAMDPROC, EXTNAME)
GLAPIPROCEXT(void, SelectPerfMonitorCountersAMD, (GLuint monitor, GLboolean enable, GLuint group, GLint numCounters, GLuint *counterList), PFNGLSELECTPERFMONITORCOUNTERSAMDPROC, EXTNAME)
GLAPIPROCEXT(void, BeginPerfMonitorAMD, (GLuint monitor), PFNGLBEGINPERFMONITORAMDPROC, EXTNAME)
GLAPIPROCEXT(void, EndPerfMonitorAMD, (GLuint monitor), PFNGLENDPERFMONITORAMDPROC, EXTNAME)
GLAPIPROCEXT(void, GetPerfMonitorCounterDataAMD, (GLuint monitor, GLenum pname, GLsizei dataSize, GLuint *data, GLint *bytesWritten), PFNGLGETPERFMONITORCOUNTERDATAAMDPROC, EXTNAME)

#undef EXTNAME /* GL_AMD_performance_monitor */
#define EXTNAME "GL_ANGLE_framebuffer_blit"
GLAPIPROCEXT(void, BlitFramebufferANGLE, (GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter), PFNGLBLITFRAMEBUFFERANGLEPROC, EXTNAME)

#undef EXTNAME /* GL_ANGLE_framebuffer_blit */
#define EXTNAME "GL_ANGLE_framebuffer_multisample"
GLAPIPROCEXT(void, RenderbufferStorageMultisampleANGLE, (GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height), PFNGLRENDERBUFFERSTORAGEMULTISAMPLEANGLEPROC, EXTNAME)

#undef EXTNAME /* GL_ANGLE_framebuffer_multisample */
#define EXTNAME "GL_ANGLE_instanced_arrays"
GLAPIPROCEXT(void, DrawArraysInstancedANGLE, (GLenum mode, GLint first, GLsizei count, GLsizei primcount), PFNGLDRAWARRAYSINSTANCEDANGLEPROC, EXTNAME)
GLAPIPROCEXT(void, DrawElementsInstancedANGLE, (GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei primcount), PFNGLDRAWELEMENTSINSTANCEDANGLEPROC, EXTNAME)
GLAPIPROCEXT(void, VertexAttribDivisorANGLE, (GLuint index, GLuint divisor), PFNGLVERTEXATTRIBDIVISORANGLEPROC, EXTNAME)

#undef EXTNAME /* GL_ANGLE_instanced_arrays */
#define EXTNAME "GL_ANGLE_translated_shader_source"
GLAPIPROCEXT(void, GetTranslatedShaderSourceANGLE, (GLuint shader, GLsizei bufsize, GLsizei *length, GLchar *source), PFNGLGETTRANSLATEDSHADERSOURCEANGLEPROC, EXTNAME)

#undef EXTNAME /* GL_ANGLE_translated_shader_source */
#define EXTNAME "GL_APPLE_copy_texture_levels"
GLAPIPROCEXT(void, CopyTextureLevelsAPPLE, (GLuint destinationTexture, GLuint sourceTexture, GLint sourceBaseLevel, GLsizei sourceLevelCount), PFNGLCOPYTEXTURELEVELSAPPLEPROC, EXTNAME)

#undef EXTNAME /* GL_APPLE_copy_texture_levels */
#define EXTNAME "GL_APPLE_framebuffer_multisample"
GLAPIPROCEXT(void, RenderbufferStorageMultisampleAPPLE, (GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height), PFNGLRENDERBUFFERSTORAGEMULTISAMPLEAPPLEPROC, EXTNAME)
GLAPIPROCEXT(void, ResolveMultisampleFramebufferAPPLE, (void), PFNGLRESOLVEMULTISAMPLEFRAMEBUFFERAPPLEPROC, EXTNAME)

#undef EXTNAME /* GL_APPLE_framebuffer_multisample */
#define EXTNAME "GL_APPLE_sync"
GLAPIFUNCEXT(GLsync, FenceSyncAPPLE, (GLenum condition, GLbitfield flags), PFNGLFENCESYNCAPPLEPROC, EXTNAME)
GLAPIFUNCEXT(GLboolean, IsSyncAPPLE, (GLsync sync), PFNGLISSYNCAPPLEPROC, EXTNAME)
GLAPIPROCEXT(void, DeleteSyncAPPLE, (GLsync sync), PFNGLDELETESYNCAPPLEPROC, EXTNAME)
GLAPIFUNCEXT(GLenum, ClientWaitSyncAPPLE, (GLsync sync, GLbitfield flags, GLuint64 timeout), PFNGLCLIENTWAITSYNCAPPLEPROC, EXTNAME)
GLAPIPROCEXT(void, WaitSyncAPPLE, (GLsync sync, GLbitfield flags, GLuint64 timeout), PFNGLWAITSYNCAPPLEPROC, EXTNAME)
GLAPIPROCEXT(void, GetInteger64vAPPLE, (GLenum pname, GLint64 *params), PFNGLGETINTEGER64VAPPLEPROC, EXTNAME)
GLAPIPROCEXT(void, GetSyncivAPPLE, (GLsync sync, GLenum pname, GLsizei bufSize, GLsizei *length, GLint *values), PFNGLGETSYNCIVAPPLEPROC, EXTNAME)

#undef EXTNAME /* GL_APPLE_sync */
#define EXTNAME "GL_EXT_base_instance"
GLAPIPROCEXT(void, DrawArraysInstancedBaseInstanceEXT, (GLenum mode, GLint first, GLsizei count, GLsizei instancecount, GLuint baseinstance), PFNGLDRAWARRAYSINSTANCEDBASEINSTANCEEXTPROC, EXTNAME)
GLAPIPROCEXT(void, DrawElementsInstancedBaseInstanceEXT, (GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount, GLuint baseinstance), PFNGLDRAWELEMENTSINSTANCEDBASEINSTANCEEXTPROC, EXTNAME)
GLAPIPROCEXT(void, DrawElementsInstancedBaseVertexBaseInstanceEXT, (GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount, GLint basevertex, GLuint baseinstance), PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCEEXTPROC, EXTNAME)

#undef EXTNAME /* GL_EXT_base_instance */
#define EXTNAME "GL_EXT_blend_func_extended"
GLAPIPROCEXT(void, BindFragDataLocationIndexedEXT, (GLuint program, GLuint colorNumber, GLuint index, const GLchar *name), PFNGLBINDFRAGDATALOCATIONINDEXEDEXTPROC, EXTNAME)
GLAPIPROCEXT(void, BindFragDataLocationEXT, (GLuint program, GLuint color, const GLchar *name), PFNGLBINDFRAGDATALOCATIONEXTPROC, EXTNAME)
GLAPIFUNCEXT(GLint, GetProgramResourceLocationIndexEXT, (GLuint program, GLenum programInterface, const GLchar *name), PFNGLGETPROGRAMRESOURCELOCATIONINDEXEXTPROC, EXTNAME)
GLAPIFUNCEXT(GLint, GetFragDataIndexEXT, (GLuint program, const GLchar *name), PFNGLGETFRAGDATAINDEXEXTPROC, EXTNAME)

#undef EXTNAME /* GL_EXT_blend_func_extended */
#define EXTNAME "GL_EXT_buffer_storage"
GLAPIPROCEXT(void, BufferStorageEXT, (GLenum target, GLsizeiptr size, const void *data, GLbitfield flags), PFNGLBUFFERSTORAGEEXTPROC, EXTNAME)

#undef EXTNAME /* GL_EXT_buffer_storage */
#define EXTNAME "GL_EXT_copy_image"
GLAPIPROCEXT(void, CopyImageSubDataEXT, (GLuint srcName, GLenum srcTarget, GLint srcLevel, GLint srcX, GLint srcY, GLint srcZ, GLuint dstName, GLenum dstTarget, GLint dstLevel, GLint dstX, GLint dstY, GLint dstZ, GLsizei srcWidth, GLsizei srcHeight, GLsizei srcDepth), PFNGLCOPYIMAGESUBDATAEXTPROC, EXTNAME)

#undef EXTNAME /* GL_EXT_copy_image */
#define EXTNAME "GL_EXT_debug_label"
GLAPIPROCEXT(void, LabelObjectEXT, (GLenum type, GLuint object, GLsizei length, const GLchar *label), PFNGLLABELOBJECTEXTPROC, EXTNAME)
GLAPIPROCEXT(void, GetObjectLabelEXT, (GLenum type, GLuint object, GLsizei bufSize, GLsizei *length, GLchar *label), PFNGLGETOBJECTLABELEXTPROC, EXTNAME)

#undef EXTNAME /* GL_EXT_debug_label */
#define EXTNAME "GL_EXT_debug_marker"
GLAPIPROCEXT(void, InsertEventMarkerEXT, (GLsizei length, const GLchar *marker), PFNGLINSERTEVENTMARKEREXTPROC, EXTNAME)
GLAPIPROCEXT(void, PushGroupMarkerEXT, (GLsizei length, const GLchar *marker), PFNGLPUSHGROUPMARKEREXTPROC, EXTNAME)
GLAPIPROCEXT(void, PopGroupMarkerEXT, (void), PFNGLPOPGROUPMARKEREXTPROC, EXTNAME)

#undef EXTNAME /* GL_EXT_debug_marker */
#define EXTNAME "GL_EXT_discard_framebuffer"
GLAPIPROCEXT(void, DiscardFramebufferEXT, (GLenum target, GLsizei numAttachments, const GLenum *attachments), PFNGLDISCARDFRAMEBUFFEREXTPROC, EXTNAME)

#undef EXTNAME /* GL_EXT_discard_framebuffer */
#define EXTNAME "GL_EXT_disjoint_timer_query"
GLAPIPROCEXT(void, GenQueriesEXT, (GLsizei n, GLuint *ids), PFNGLGENQUERIESEXTPROC, EXTNAME)
GLAPIPROCEXT(void, DeleteQueriesEXT, (GLsizei n, const GLuint *ids), PFNGLDELETEQUERIESEXTPROC, EXTNAME)
GLAPIFUNCEXT(GLboolean, IsQueryEXT, (GLuint id), PFNGLISQUERYEXTPROC, EXTNAME)
GLAPIPROCEXT(void, BeginQueryEXT, (GLenum target, GLuint id), PFNGLBEGINQUERYEXTPROC, EXTNAME)
GLAPIPROCEXT(void, EndQueryEXT, (GLenum target), PFNGLENDQUERYEXTPROC, EXTNAME)
GLAPIPROCEXT(void, QueryCounterEXT, (GLuint id, GLenum target), PFNGLQUERYCOUNTEREXTPROC, EXTNAME)
GLAPIPROCEXT(void, GetQueryivEXT, (GLenum target, GLenum pname, GLint *params), PFNGLGETQUERYIVEXTPROC, EXTNAME)
GLAPIPROCEXT(void, GetQueryObjectivEXT, (GLuint id, GLenum pname, GLint *params), PFNGLGETQUERYOBJECTIVEXTPROC, EXTNAME)
GLAPIPROCEXT(void, GetQueryObjectuivEXT, (GLuint id, GLenum pname, GLuint *params), PFNGLGETQUERYOBJECTUIVEXTPROC, EXTNAME)
GLAPIPROCEXT(void, GetQueryObjecti64vEXT, (GLuint id, GLenum pname, GLint64 *params), PFNGLGETQUERYOBJECTI64VEXTPROC, EXTNAME)
GLAPIPROCEXT(void, GetQueryObjectui64vEXT, (GLuint id, GLenum pname, GLuint64 *params), PFNGLGETQUERYOBJECTUI64VEXTPROC, EXTNAME)

#undef EXTNAME /* GL_EXT_disjoint_timer_query */
#define EXTNAME "GL_EXT_draw_buffers"
GLAPIPROCEXT(void, DrawBuffersEXT, (GLsizei n, const GLenum *bufs), PFNGLDRAWBUFFERSEXTPROC, EXTNAME)

#undef EXTNAME /* GL_EXT_draw_buffers */
#define EXTNAME "GL_EXT_draw_buffers_indexed"
GLAPIPROCEXT(void, EnableiEXT, (GLenum target, GLuint index), PFNGLENABLEIEXTPROC, EXTNAME)
GLAPIPROCEXT(void, DisableiEXT, (GLenum target, GLuint index), PFNGLDISABLEIEXTPROC, EXTNAME)
GLAPIPROCEXT(void, BlendEquationiEXT, (GLuint buf, GLenum mode), PFNGLBLENDEQUATIONIEXTPROC, EXTNAME)
GLAPIPROCEXT(void, BlendEquationSeparateiEXT, (GLuint buf, GLenum modeRGB, GLenum modeAlpha), PFNGLBLENDEQUATIONSEPARATEIEXTPROC, EXTNAME)
GLAPIPROCEXT(void, BlendFunciEXT, (GLuint buf, GLenum src, GLenum dst), PFNGLBLENDFUNCIEXTPROC, EXTNAME)
GLAPIPROCEXT(void, BlendFuncSeparateiEXT, (GLuint buf, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha), PFNGLBLENDFUNCSEPARATEIEXTPROC, EXTNAME)
GLAPIPROCEXT(void, ColorMaskiEXT, (GLuint index, GLboolean r, GLboolean g, GLboolean b, GLboolean a), PFNGLCOLORMASKIEXTPROC, EXTNAME)
GLAPIFUNCEXT(GLboolean, IsEnablediEXT, (GLenum target, GLuint index), PFNGLISENABLEDIEXTPROC, EXTNAME)

#undef EXTNAME /* GL_EXT_draw_buffers_indexed */
#define EXTNAME "GL_EXT_draw_elements_base_vertex"
GLAPIPROCEXT(void, DrawElementsBaseVertexEXT, (GLenum mode, GLsizei count, GLenum type, const void *indices, GLint basevertex), PFNGLDRAWELEMENTSBASEVERTEXEXTPROC, EXTNAME)
GLAPIPROCEXT(void, DrawRangeElementsBaseVertexEXT, (GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void *indices, GLint basevertex), PFNGLDRAWRANGEELEMENTSBASEVERTEXEXTPROC, EXTNAME)
GLAPIPROCEXT(void, DrawElementsInstancedBaseVertexEXT, (GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount, GLint basevertex), PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXEXTPROC, EXTNAME)
GLAPIPROCEXT(void, MultiDrawElementsBaseVertexEXT, (GLenum mode, const GLsizei *count, GLenum type, const void *const*indices, GLsizei primcount, const GLint *basevertex), PFNGLMULTIDRAWELEMENTSBASEVERTEXEXTPROC, EXTNAME)

#undef EXTNAME /* GL_EXT_draw_elements_base_vertex */
#define EXTNAME "GL_EXT_draw_instanced"
GLAPIPROCEXT(void, DrawArraysInstancedEXT, (GLenum mode, GLint start, GLsizei count, GLsizei primcount), PFNGLDRAWARRAYSINSTANCEDEXTPROC, EXTNAME)
GLAPIPROCEXT(void, DrawElementsInstancedEXT, (GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei primcount), PFNGLDRAWELEMENTSINSTANCEDEXTPROC, EXTNAME)

#undef EXTNAME /* GL_EXT_draw_instanced */
#define EXTNAME "GL_EXT_geometry_shader"
GLAPIPROCEXT(void, FramebufferTextureEXT, (GLenum target, GLenum attachment, GLuint texture, GLint level), PFNGLFRAMEBUFFERTEXTUREEXTPROC, EXTNAME)

#undef EXTNAME /* GL_EXT_geometry_shader */
#define EXTNAME "GL_EXT_instanced_arrays"
GLAPIPROCEXT(void, VertexAttribDivisorEXT, (GLuint index, GLuint divisor), PFNGLVERTEXATTRIBDIVISOREXTPROC, EXTNAME)

#undef EXTNAME /* GL_EXT_instanced_arrays */
#define EXTNAME "GL_EXT_map_buffer_range"
GLAPIFUNCEXT(void *, MapBufferRangeEXT, (GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access), PFNGLMAPBUFFERRANGEEXTPROC, EXTNAME)
GLAPIPROCEXT(void, FlushMappedBufferRangeEXT, (GLenum target, GLintptr offset, GLsizeiptr length), PFNGLFLUSHMAPPEDBUFFERRANGEEXTPROC, EXTNAME)

#undef EXTNAME /* GL_EXT_map_buffer_range */
#define EXTNAME "GL_EXT_multi_draw_arrays"
GLAPIPROCEXT(void, MultiDrawArraysEXT, (GLenum mode, const GLint *first, const GLsizei *count, GLsizei primcount), PFNGLMULTIDRAWARRAYSEXTPROC, EXTNAME)
GLAPIPROCEXT(void, MultiDrawElementsEXT, (GLenum mode, const GLsizei *count, GLenum type, const void *const*indices, GLsizei primcount), PFNGLMULTIDRAWELEMENTSEXTPROC, EXTNAME)

#undef EXTNAME /* GL_EXT_multi_draw_arrays */
#define EXTNAME "GL_EXT_multi_draw_indirect"
GLAPIPROCEXT(void, MultiDrawArraysIndirectEXT, (GLenum mode, const void *indirect, GLsizei drawcount, GLsizei stride), PFNGLMULTIDRAWARRAYSINDIRECTEXTPROC, EXTNAME)
GLAPIPROCEXT(void, MultiDrawElementsIndirectEXT, (GLenum mode, GLenum type, const void *indirect, GLsizei drawcount, GLsizei stride), PFNGLMULTIDRAWELEMENTSINDIRECTEXTPROC, EXTNAME)

#undef EXTNAME /* GL_EXT_multi_draw_indirect */
#define EXTNAME "GL_EXT_multisampled_render_to_texture"
GLAPIPROCEXT(void, RenderbufferStorageMultisampleEXT, (GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height), PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC, EXTNAME)
GLAPIPROCEXT(void, FramebufferTexture2DMultisampleEXT, (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLsizei samples), PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEEXTPROC, EXTNAME)

#undef EXTNAME /* GL_EXT_multisampled_render_to_texture */
#define EXTNAME "GL_EXT_multiview_draw_buffers"
GLAPIPROCEXT(void, ReadBufferIndexedEXT, (GLenum src, GLint index), PFNGLREADBUFFERINDEXEDEXTPROC, EXTNAME)
GLAPIPROCEXT(void, DrawBuffersIndexedEXT, (GLint n, const GLenum *location, const GLint *indices), PFNGLDRAWBUFFERSINDEXEDEXTPROC, EXTNAME)
GLAPIPROCEXT(void, GetIntegeri_vEXT, (GLenum target, GLuint index, GLint *data), PFNGLGETINTEGERI_VEXTPROC, EXTNAME)

#undef EXTNAME /* GL_EXT_multiview_draw_buffers */
#define EXTNAME "GL_EXT_polygon_offset_clamp"
GLAPIPROCEXT(void, PolygonOffsetClampEXT, (GLfloat factor, GLfloat units, GLfloat clamp), PFNGLPOLYGONOFFSETCLAMPEXTPROC, EXTNAME)

#undef EXTNAME /* GL_EXT_polygon_offset_clamp */
#define EXTNAME "GL_EXT_primitive_bounding_box"
GLAPIPROCEXT(void, PrimitiveBoundingBoxEXT, (GLfloat minX, GLfloat minY, GLfloat minZ, GLfloat minW, GLfloat maxX, GLfloat maxY, GLfloat maxZ, GLfloat maxW), PFNGLPRIMITIVEBOUNDINGBOXEXTPROC, EXTNAME)

#undef EXTNAME /* GL_EXT_primitive_bounding_box */
#define EXTNAME "GL_EXT_raster_multisample"
GLAPIPROCEXT(void, RasterSamplesEXT, (GLuint samples, GLboolean fixedsamplelocations), PFNGLRASTERSAMPLESEXTPROC, EXTNAME)

#undef EXTNAME /* GL_EXT_raster_multisample */
#define EXTNAME "GL_EXT_robustness"
GLAPIFUNCEXT(GLenum, GetGraphicsResetStatusEXT, (void), PFNGLGETGRAPHICSRESETSTATUSEXTPROC, EXTNAME)
GLAPIPROCEXT(void, ReadnPixelsEXT, (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLsizei bufSize, void *data), PFNGLREADNPIXELSEXTPROC, EXTNAME)
GLAPIPROCEXT(void, GetnUniformfvEXT, (GLuint program, GLint location, GLsizei bufSize, GLfloat *params), PFNGLGETNUNIFORMFVEXTPROC, EXTNAME)
GLAPIPROCEXT(void, GetnUniformivEXT, (GLuint program, GLint location, GLsizei bufSize, GLint *params), PFNGLGETNUNIFORMIVEXTPROC, EXTNAME)

#undef EXTNAME /* GL_EXT_robustness */
#define EXTNAME "GL_EXT_separate_shader_objects"
GLAPIPROCEXT(void, ActiveShaderProgramEXT, (GLuint pipeline, GLuint program), PFNGLACTIVESHADERPROGRAMEXTPROC, EXTNAME)
GLAPIPROCEXT(void, BindProgramPipelineEXT, (GLuint pipeline), PFNGLBINDPROGRAMPIPELINEEXTPROC, EXTNAME)
GLAPIFUNCEXT(GLuint, CreateShaderProgramvEXT, (GLenum type, GLsizei count, const GLchar **strings), PFNGLCREATESHADERPROGRAMVEXTPROC, EXTNAME)
GLAPIPROCEXT(void, DeleteProgramPipelinesEXT, (GLsizei n, const GLuint *pipelines), PFNGLDELETEPROGRAMPIPELINESEXTPROC, EXTNAME)
GLAPIPROCEXT(void, GenProgramPipelinesEXT, (GLsizei n, GLuint *pipelines), PFNGLGENPROGRAMPIPELINESEXTPROC, EXTNAME)
GLAPIPROCEXT(void, GetProgramPipelineInfoLogEXT, (GLuint pipeline, GLsizei bufSize, GLsizei *length, GLchar *infoLog), PFNGLGETPROGRAMPIPELINEINFOLOGEXTPROC, EXTNAME)
GLAPIPROCEXT(void, GetProgramPipelineivEXT, (GLuint pipeline, GLenum pname, GLint *params), PFNGLGETPROGRAMPIPELINEIVEXTPROC, EXTNAME)
GLAPIFUNCEXT(GLboolean, IsProgramPipelineEXT, (GLuint pipeline), PFNGLISPROGRAMPIPELINEEXTPROC, EXTNAME)
GLAPIPROCEXT(void, ProgramParameteriEXT, (GLuint program, GLenum pname, GLint value), PFNGLPROGRAMPARAMETERIEXTPROC, EXTNAME)
GLAPIPROCEXT(void, ProgramUniform1fEXT, (GLuint program, GLint location, GLfloat v0), PFNGLPROGRAMUNIFORM1FEXTPROC, EXTNAME)
GLAPIPROCEXT(void, ProgramUniform1fvEXT, (GLuint program, GLint location, GLsizei count, const GLfloat *value), PFNGLPROGRAMUNIFORM1FVEXTPROC, EXTNAME)
GLAPIPROCEXT(void, ProgramUniform1iEXT, (GLuint program, GLint location, GLint v0), PFNGLPROGRAMUNIFORM1IEXTPROC, EXTNAME)
GLAPIPROCEXT(void, ProgramUniform1ivEXT, (GLuint program, GLint location, GLsizei count, const GLint *value), PFNGLPROGRAMUNIFORM1IVEXTPROC, EXTNAME)
GLAPIPROCEXT(void, ProgramUniform2fEXT, (GLuint program, GLint location, GLfloat v0, GLfloat v1), PFNGLPROGRAMUNIFORM2FEXTPROC, EXTNAME)
GLAPIPROCEXT(void, ProgramUniform2fvEXT, (GLuint program, GLint location, GLsizei count, const GLfloat *value), PFNGLPROGRAMUNIFORM2FVEXTPROC, EXTNAME)
GLAPIPROCEXT(void, ProgramUniform2iEXT, (GLuint program, GLint location, GLint v0, GLint v1), PFNGLPROGRAMUNIFORM2IEXTPROC, EXTNAME)
GLAPIPROCEXT(void, ProgramUniform2ivEXT, (GLuint program, GLint location, GLsizei count, const GLint *value), PFNGLPROGRAMUNIFORM2IVEXTPROC, EXTNAME)
GLAPIPROCEXT(void, ProgramUniform3fEXT, (GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2), PFNGLPROGRAMUNIFORM3FEXTPROC, EXTNAME)
GLAPIPROCEXT(void, ProgramUniform3fvEXT, (GLuint program, GLint location, GLsizei count, const GLfloat *value), PFNGLPROGRAMUNIFORM3FVEXTPROC, EXTNAME)
GLAPIPROCEXT(void, ProgramUniform3iEXT, (GLuint program, GLint location, GLint v0, GLint v1, GLint v2), PFNGLPROGRAMUNIFORM3IEXTPROC, EXTNAME)
GLAPIPROCEXT(void, ProgramUniform3ivEXT, (GLuint program, GLint location, GLsizei count, const GLint *value), PFNGLPROGRAMUNIFORM3IVEXTPROC, EXTNAME)
GLAPIPROCEXT(void, ProgramUniform4fEXT, (GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3), PFNGLPROGRAMUNIFORM4FEXTPROC, EXTNAME)
GLAPIPROCEXT(void, ProgramUniform4fvEXT, (GLuint program, GLint location, GLsizei count, const GLfloat *value), PFNGLPROGRAMUNIFORM4FVEXTPROC, EXTNAME)
GLAPIPROCEXT(void, ProgramUniform4iEXT, (GLuint program, GLint location, GLint v0, GLint v1, GLint v2, GLint v3), PFNGLPROGRAMUNIFORM4IEXTPROC, EXTNAME)
GLAPIPROCEXT(void, ProgramUniform4ivEXT, (GLuint program, GLint location, GLsizei count, const GLint *value), PFNGLPROGRAMUNIFORM4IVEXTPROC, EXTNAME)
GLAPIPROCEXT(void, ProgramUniformMatrix2fvEXT, (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value), PFNGLPROGRAMUNIFORMMATRIX2FVEXTPROC, EXTNAME)
GLAPIPROCEXT(void, ProgramUniformMatrix3fvEXT, (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value), PFNGLPROGRAMUNIFORMMATRIX3FVEXTPROC, EXTNAME)
GLAPIPROCEXT(void, ProgramUniformMatrix4fvEXT, (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value), PFNGLPROGRAMUNIFORMMATRIX4FVEXTPROC, EXTNAME)
GLAPIPROCEXT(void, UseProgramStagesEXT, (GLuint pipeline, GLbitfield stages, GLuint program), PFNGLUSEPROGRAMSTAGESEXTPROC, EXTNAME)
GLAPIPROCEXT(void, ValidateProgramPipelineEXT, (GLuint pipeline), PFNGLVALIDATEPROGRAMPIPELINEEXTPROC, EXTNAME)
GLAPIPROCEXT(void, ProgramUniform1uiEXT, (GLuint program, GLint location, GLuint v0), PFNGLPROGRAMUNIFORM1UIEXTPROC, EXTNAME)
GLAPIPROCEXT(void, ProgramUniform2uiEXT, (GLuint program, GLint location, GLuint v0, GLuint v1), PFNGLPROGRAMUNIFORM2UIEXTPROC, EXTNAME)
GLAPIPROCEXT(void, ProgramUniform3uiEXT, (GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2), PFNGLPROGRAMUNIFORM3UIEXTPROC, EXTNAME)
GLAPIPROCEXT(void, ProgramUniform4uiEXT, (GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3), PFNGLPROGRAMUNIFORM4UIEXTPROC, EXTNAME)
GLAPIPROCEXT(void, ProgramUniform1uivEXT, (GLuint program, GLint location, GLsizei count, const GLuint *value), PFNGLPROGRAMUNIFORM1UIVEXTPROC, EXTNAME)
GLAPIPROCEXT(void, ProgramUniform2uivEXT, (GLuint program, GLint location, GLsizei count, const GLuint *value), PFNGLPROGRAMUNIFORM2UIVEXTPROC, EXTNAME)
GLAPIPROCEXT(void, ProgramUniform3uivEXT, (GLuint program, GLint location, GLsizei count, const GLuint *value), PFNGLPROGRAMUNIFORM3UIVEXTPROC, EXTNAME)
GLAPIPROCEXT(void, ProgramUniform4uivEXT, (GLuint program, GLint location, GLsizei count, const GLuint *value), PFNGLPROGRAMUNIFORM4UIVEXTPROC, EXTNAME)
GLAPIPROCEXT(void, ProgramUniformMatrix2x3fvEXT, (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value), PFNGLPROGRAMUNIFORMMATRIX2X3FVEXTPROC, EXTNAME)
GLAPIPROCEXT(void, ProgramUniformMatrix3x2fvEXT, (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value), PFNGLPROGRAMUNIFORMMATRIX3X2FVEXTPROC, EXTNAME)
GLAPIPROCEXT(void, ProgramUniformMatrix2x4fvEXT, (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value), PFNGLPROGRAMUNIFORMMATRIX2X4FVEXTPROC, EXTNAME)
GLAPIPROCEXT(void, ProgramUniformMatrix4x2fvEXT, (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value), PFNGLPROGRAMUNIFORMMATRIX4X2FVEXTPROC, EXTNAME)
GLAPIPROCEXT(void, ProgramUniformMatrix3x4fvEXT, (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value), PFNGLPROGRAMUNIFORMMATRIX3X4FVEXTPROC, EXTNAME)
GLAPIPROCEXT(void, ProgramUniformMatrix4x3fvEXT, (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value), PFNGLPROGRAMUNIFORMMATRIX4X3FVEXTPROC, EXTNAME)

#undef EXTNAME /* GL_EXT_separate_shader_objects */
#define EXTNAME "GL_EXT_shader_pixel_local_storage2"
GLAPIPROCEXT(void, FramebufferPixelLocalStorageSizeEXT, (GLuint target, GLsizei size), PFNGLFRAMEBUFFERPIXELLOCALSTORAGESIZEEXTPROC, EXTNAME)
GLAPIFUNCEXT(GLsizei, GetFramebufferPixelLocalStorageSizeEXT, (GLuint target), PFNGLGETFRAMEBUFFERPIXELLOCALSTORAGESIZEEXTPROC, EXTNAME)
GLAPIPROCEXT(void, ClearPixelLocalStorageuiEXT, (GLsizei offset, GLsizei n, const GLuint *values), PFNGLCLEARPIXELLOCALSTORAGEUIEXTPROC, EXTNAME)

#undef EXTNAME /* GL_EXT_shader_pixel_local_storage2 */
#define EXTNAME "GL_EXT_sparse_texture"
GLAPIPROCEXT(void, TexPageCommitmentEXT, (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLboolean commit), PFNGLTEXPAGECOMMITMENTEXTPROC, EXTNAME)

#undef EXTNAME /* GL_EXT_sparse_texture */
#define EXTNAME "GL_EXT_tessellation_shader"
GLAPIPROCEXT(void, PatchParameteriEXT, (GLenum pname, GLint value), PFNGLPATCHPARAMETERIEXTPROC, EXTNAME)

#undef EXTNAME /* GL_EXT_tessellation_shader */
#define EXTNAME "GL_EXT_texture_border_clamp"
GLAPIPROCEXT(void, TexParameterIivEXT, (GLenum target, GLenum pname, const GLint *params), PFNGLTEXPARAMETERIIVEXTPROC, EXTNAME)
GLAPIPROCEXT(void, TexParameterIuivEXT, (GLenum target, GLenum pname, const GLuint *params), PFNGLTEXPARAMETERIUIVEXTPROC, EXTNAME)
GLAPIPROCEXT(void, GetTexParameterIivEXT, (GLenum target, GLenum pname, GLint *params), PFNGLGETTEXPARAMETERIIVEXTPROC, EXTNAME)
GLAPIPROCEXT(void, GetTexParameterIuivEXT, (GLenum target, GLenum pname, GLuint *params), PFNGLGETTEXPARAMETERIUIVEXTPROC, EXTNAME)
GLAPIPROCEXT(void, SamplerParameterIivEXT, (GLuint sampler, GLenum pname, const GLint *param), PFNGLSAMPLERPARAMETERIIVEXTPROC, EXTNAME)
GLAPIPROCEXT(void, SamplerParameterIuivEXT, (GLuint sampler, GLenum pname, const GLuint *param), PFNGLSAMPLERPARAMETERIUIVEXTPROC, EXTNAME)
GLAPIPROCEXT(void, GetSamplerParameterIivEXT, (GLuint sampler, GLenum pname, GLint *params), PFNGLGETSAMPLERPARAMETERIIVEXTPROC, EXTNAME)
GLAPIPROCEXT(void, GetSamplerParameterIuivEXT, (GLuint sampler, GLenum pname, GLuint *params), PFNGLGETSAMPLERPARAMETERIUIVEXTPROC, EXTNAME)

#undef EXTNAME /* GL_EXT_texture_border_clamp */
#define EXTNAME "GL_EXT_texture_buffer"
GLAPIPROCEXT(void, TexBufferEXT, (GLenum target, GLenum internalformat, GLuint buffer), PFNGLTEXBUFFEREXTPROC, EXTNAME)
GLAPIPROCEXT(void, TexBufferRangeEXT, (GLenum target, GLenum internalformat, GLuint buffer, GLintptr offset, GLsizeiptr size), PFNGLTEXBUFFERRANGEEXTPROC, EXTNAME)

#undef EXTNAME /* GL_EXT_texture_buffer */
#define EXTNAME "GL_EXT_texture_storage"
GLAPIPROCEXT(void, TexStorage1DEXT, (GLenum target, GLsizei levels, GLenum internalformat, GLsizei width), PFNGLTEXSTORAGE1DEXTPROC, EXTNAME)
GLAPIPROCEXT(void, TexStorage2DEXT, (GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height), PFNGLTEXSTORAGE2DEXTPROC, EXTNAME)
GLAPIPROCEXT(void, TexStorage3DEXT, (GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth), PFNGLTEXSTORAGE3DEXTPROC, EXTNAME)
GLAPIPROCEXT(void, TextureStorage1DEXT, (GLuint texture, GLenum target, GLsizei levels, GLenum internalformat, GLsizei width), PFNGLTEXTURESTORAGE1DEXTPROC, EXTNAME)
GLAPIPROCEXT(void, TextureStorage2DEXT, (GLuint texture, GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height), PFNGLTEXTURESTORAGE2DEXTPROC, EXTNAME)
GLAPIPROCEXT(void, TextureStorage3DEXT, (GLuint texture, GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth), PFNGLTEXTURESTORAGE3DEXTPROC, EXTNAME)

#undef EXTNAME /* GL_EXT_texture_storage */
#define EXTNAME "GL_EXT_texture_view"
GLAPIPROCEXT(void, TextureViewEXT, (GLuint texture, GLenum target, GLuint origtexture, GLenum internalformat, GLuint minlevel, GLuint numlevels, GLuint minlayer, GLuint numlayers), PFNGLTEXTUREVIEWEXTPROC, EXTNAME)

#undef EXTNAME /* GL_EXT_texture_view */
#define EXTNAME "GL_IMG_framebuffer_downsample"
GLAPIPROCEXT(void, FramebufferTexture2DDownsampleIMG, (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint xscale, GLint yscale), PFNGLFRAMEBUFFERTEXTURE2DDOWNSAMPLEIMGPROC, EXTNAME)
GLAPIPROCEXT(void, FramebufferTextureLayerDownsampleIMG, (GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer, GLint xscale, GLint yscale), PFNGLFRAMEBUFFERTEXTURELAYERDOWNSAMPLEIMGPROC, EXTNAME)

#undef EXTNAME /* GL_IMG_framebuffer_downsample */
#define EXTNAME "GL_IMG_multisampled_render_to_texture"
GLAPIPROCEXT(void, RenderbufferStorageMultisampleIMG, (GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height), PFNGLRENDERBUFFERSTORAGEMULTISAMPLEIMGPROC, EXTNAME)
GLAPIPROCEXT(void, FramebufferTexture2DMultisampleIMG, (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLsizei samples), PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEIMGPROC, EXTNAME)

#undef EXTNAME /* GL_IMG_multisampled_render_to_texture */
#define EXTNAME "GL_INTEL_framebuffer_CMAA"
GLAPIPROCEXT(void, ApplyFramebufferAttachmentCMAAINTEL, (void), PFNGLAPPLYFRAMEBUFFERATTACHMENTCMAAINTELPROC, EXTNAME)

#undef EXTNAME /* GL_INTEL_framebuffer_CMAA */
#define EXTNAME "GL_INTEL_performance_query"
GLAPIPROCEXT(void, BeginPerfQueryINTEL, (GLuint queryHandle), PFNGLBEGINPERFQUERYINTELPROC, EXTNAME)
GLAPIPROCEXT(void, CreatePerfQueryINTEL, (GLuint queryId, GLuint *queryHandle), PFNGLCREATEPERFQUERYINTELPROC, EXTNAME)
GLAPIPROCEXT(void, DeletePerfQueryINTEL, (GLuint queryHandle), PFNGLDELETEPERFQUERYINTELPROC, EXTNAME)
GLAPIPROCEXT(void, EndPerfQueryINTEL, (GLuint queryHandle), PFNGLENDPERFQUERYINTELPROC, EXTNAME)
GLAPIPROCEXT(void, GetFirstPerfQueryIdINTEL, (GLuint *queryId), PFNGLGETFIRSTPERFQUERYIDINTELPROC, EXTNAME)
GLAPIPROCEXT(void, GetNextPerfQueryIdINTEL, (GLuint queryId, GLuint *nextQueryId), PFNGLGETNEXTPERFQUERYIDINTELPROC, EXTNAME)
GLAPIPROCEXT(void, GetPerfCounterInfoINTEL, (GLuint queryId, GLuint counterId, GLuint counterNameLength, GLchar *counterName, GLuint counterDescLength, GLchar *counterDesc, GLuint *counterOffset, GLuint *counterDataSize, GLuint *counterTypeEnum, GLuint *counterDataTypeEnum, GLuint64 *rawCounterMaxValue), PFNGLGETPERFCOUNTERINFOINTELPROC, EXTNAME)
GLAPIPROCEXT(void, GetPerfQueryDataINTEL, (GLuint queryHandle, GLuint flags, GLsizei dataSize, GLvoid *data, GLuint *bytesWritten), PFNGLGETPERFQUERYDATAINTELPROC, EXTNAME)
GLAPIPROCEXT(void, GetPerfQueryIdByNameINTEL, (GLchar *queryName, GLuint *queryId), PFNGLGETPERFQUERYIDBYNAMEINTELPROC, EXTNAME)
GLAPIPROCEXT(void, GetPerfQueryInfoINTEL, (GLuint queryId, GLuint queryNameLength, GLchar *queryName, GLuint *dataSize, GLuint *noCounters, GLuint *noInstances, GLuint *capsMask), PFNGLGETPERFQUERYINFOINTELPROC, EXTNAME)

#undef EXTNAME /* GL_INTEL_performance_query */
#define EXTNAME "GL_NV_bindless_texture"
GLAPIFUNCEXT(GLuint64, GetTextureHandleNV, (GLuint texture), PFNGLGETTEXTUREHANDLENVPROC, EXTNAME)
GLAPIFUNCEXT(GLuint64, GetTextureSamplerHandleNV, (GLuint texture, GLuint sampler), PFNGLGETTEXTURESAMPLERHANDLENVPROC, EXTNAME)
GLAPIPROCEXT(void, MakeTextureHandleResidentNV, (GLuint64 handle), PFNGLMAKETEXTUREHANDLERESIDENTNVPROC, EXTNAME)
GLAPIPROCEXT(void, MakeTextureHandleNonResidentNV, (GLuint64 handle), PFNGLMAKETEXTUREHANDLENONRESIDENTNVPROC, EXTNAME)
GLAPIFUNCEXT(GLuint64, GetImageHandleNV, (GLuint texture, GLint level, GLboolean layered, GLint layer, GLenum format), PFNGLGETIMAGEHANDLENVPROC, EXTNAME)
GLAPIPROCEXT(void, MakeImageHandleResidentNV, (GLuint64 handle, GLenum access), PFNGLMAKEIMAGEHANDLERESIDENTNVPROC, EXTNAME)
GLAPIPROCEXT(void, MakeImageHandleNonResidentNV, (GLuint64 handle), PFNGLMAKEIMAGEHANDLENONRESIDENTNVPROC, EXTNAME)
GLAPIPROCEXT(void, UniformHandleui64NV, (GLint location, GLuint64 value), PFNGLUNIFORMHANDLEUI64NVPROC, EXTNAME)
GLAPIPROCEXT(void, UniformHandleui64vNV, (GLint location, GLsizei count, const GLuint64 *value), PFNGLUNIFORMHANDLEUI64VNVPROC, EXTNAME)
GLAPIPROCEXT(void, ProgramUniformHandleui64NV, (GLuint program, GLint location, GLuint64 value), PFNGLPROGRAMUNIFORMHANDLEUI64NVPROC, EXTNAME)
GLAPIPROCEXT(void, ProgramUniformHandleui64vNV, (GLuint program, GLint location, GLsizei count, const GLuint64 *values), PFNGLPROGRAMUNIFORMHANDLEUI64VNVPROC, EXTNAME)
GLAPIFUNCEXT(GLboolean, IsTextureHandleResidentNV, (GLuint64 handle), PFNGLISTEXTUREHANDLERESIDENTNVPROC, EXTNAME)
GLAPIFUNCEXT(GLboolean, IsImageHandleResidentNV, (GLuint64 handle), PFNGLISIMAGEHANDLERESIDENTNVPROC, EXTNAME)

#undef EXTNAME /* GL_NV_bindless_texture */
#define EXTNAME "GL_NV_blend_equation_advanced"
GLAPIPROCEXT(void, BlendParameteriNV, (GLenum pname, GLint value), PFNGLBLENDPARAMETERINVPROC, EXTNAME)
GLAPIPROCEXT(void, BlendBarrierNV, (void), PFNGLBLENDBARRIERNVPROC, EXTNAME)

#undef EXTNAME /* GL_NV_blend_equation_advanced */
#define EXTNAME "GL_NV_conditional_render"
GLAPIPROCEXT(void, BeginConditionalRenderNV, (GLuint id, GLenum mode), PFNGLBEGINCONDITIONALRENDERNVPROC, EXTNAME)
GLAPIPROCEXT(void, EndConditionalRenderNV, (void), PFNGLENDCONDITIONALRENDERNVPROC, EXTNAME)

#undef EXTNAME /* GL_NV_conditional_render */
#define EXTNAME "GL_NV_conservative_raster"
GLAPIPROCEXT(void, SubpixelPrecisionBiasNV, (GLuint xbits, GLuint ybits), PFNGLSUBPIXELPRECISIONBIASNVPROC, EXTNAME)

#undef EXTNAME /* GL_NV_conservative_raster */
#define EXTNAME "GL_NV_copy_buffer"
GLAPIPROCEXT(void, CopyBufferSubDataNV, (GLenum readTarget, GLenum writeTarget, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size), PFNGLCOPYBUFFERSUBDATANVPROC, EXTNAME)

#undef EXTNAME /* GL_NV_copy_buffer */
#define EXTNAME "GL_NV_coverage_sample"
GLAPIPROCEXT(void, CoverageMaskNV, (GLboolean mask), PFNGLCOVERAGEMASKNVPROC, EXTNAME)
GLAPIPROCEXT(void, CoverageOperationNV, (GLenum operation), PFNGLCOVERAGEOPERATIONNVPROC, EXTNAME)

#undef EXTNAME /* GL_NV_coverage_sample */
#define EXTNAME "GL_NV_draw_buffers"
GLAPIPROCEXT(void, DrawBuffersNV, (GLsizei n, const GLenum *bufs), PFNGLDRAWBUFFERSNVPROC, EXTNAME)

#undef EXTNAME /* GL_NV_draw_buffers */
#define EXTNAME "GL_NV_draw_instanced"
GLAPIPROCEXT(void, DrawArraysInstancedNV, (GLenum mode, GLint first, GLsizei count, GLsizei primcount), PFNGLDRAWARRAYSINSTANCEDNVPROC, EXTNAME)
GLAPIPROCEXT(void, DrawElementsInstancedNV, (GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei primcount), PFNGLDRAWELEMENTSINSTANCEDNVPROC, EXTNAME)

#undef EXTNAME /* GL_NV_draw_instanced */
#define EXTNAME "GL_NV_fence"
GLAPIPROCEXT(void, DeleteFencesNV, (GLsizei n, const GLuint *fences), PFNGLDELETEFENCESNVPROC, EXTNAME)
GLAPIPROCEXT(void, GenFencesNV, (GLsizei n, GLuint *fences), PFNGLGENFENCESNVPROC, EXTNAME)
GLAPIFUNCEXT(GLboolean, IsFenceNV, (GLuint fence), PFNGLISFENCENVPROC, EXTNAME)
GLAPIFUNCEXT(GLboolean, TestFenceNV, (GLuint fence), PFNGLTESTFENCENVPROC, EXTNAME)
GLAPIPROCEXT(void, GetFenceivNV, (GLuint fence, GLenum pname, GLint *params), PFNGLGETFENCEIVNVPROC, EXTNAME)
GLAPIPROCEXT(void, FinishFenceNV, (GLuint fence), PFNGLFINISHFENCENVPROC, EXTNAME)
GLAPIPROCEXT(void, SetFenceNV, (GLuint fence, GLenum condition), PFNGLSETFENCENVPROC, EXTNAME)

#undef EXTNAME /* GL_NV_fence */
#define EXTNAME "GL_NV_fragment_coverage_to_color"
GLAPIPROCEXT(void, FragmentCoverageColorNV, (GLuint color), PFNGLFRAGMENTCOVERAGECOLORNVPROC, EXTNAME)

#undef EXTNAME /* GL_NV_fragment_coverage_to_color */
#define EXTNAME "GL_NV_framebuffer_blit"
GLAPIPROCEXT(void, BlitFramebufferNV, (GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter), PFNGLBLITFRAMEBUFFERNVPROC, EXTNAME)

#undef EXTNAME /* GL_NV_framebuffer_blit */
#define EXTNAME "GL_NV_framebuffer_mixed_samples"
GLAPIPROCEXT(void, CoverageModulationTableNV, (GLsizei n, const GLfloat *v), PFNGLCOVERAGEMODULATIONTABLENVPROC, EXTNAME)
GLAPIPROCEXT(void, GetCoverageModulationTableNV, (GLsizei bufsize, GLfloat *v), PFNGLGETCOVERAGEMODULATIONTABLENVPROC, EXTNAME)
GLAPIPROCEXT(void, CoverageModulationNV, (GLenum components), PFNGLCOVERAGEMODULATIONNVPROC, EXTNAME)

#undef EXTNAME /* GL_NV_framebuffer_mixed_samples */
#define EXTNAME "GL_NV_framebuffer_multisample"
GLAPIPROCEXT(void, RenderbufferStorageMultisampleNV, (GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height), PFNGLRENDERBUFFERSTORAGEMULTISAMPLENVPROC, EXTNAME)

#undef EXTNAME /* GL_NV_framebuffer_multisample */
#define EXTNAME "GL_NV_instanced_arrays"
GLAPIPROCEXT(void, VertexAttribDivisorNV, (GLuint index, GLuint divisor), PFNGLVERTEXATTRIBDIVISORNVPROC, EXTNAME)

#undef EXTNAME /* GL_NV_instanced_arrays */
#define EXTNAME "GL_NV_internalformat_sample_query"
GLAPIPROCEXT(void, GetInternalformatSampleivNV, (GLenum target, GLenum internalformat, GLsizei samples, GLenum pname, GLsizei bufSize, GLint *params), PFNGLGETINTERNALFORMATSAMPLEIVNVPROC, EXTNAME)

#undef EXTNAME /* GL_NV_internalformat_sample_query */
#define EXTNAME "GL_NV_non_square_matrices"
GLAPIPROCEXT(void, UniformMatrix2x3fvNV, (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value), PFNGLUNIFORMMATRIX2X3FVNVPROC, EXTNAME)
GLAPIPROCEXT(void, UniformMatrix3x2fvNV, (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value), PFNGLUNIFORMMATRIX3X2FVNVPROC, EXTNAME)
GLAPIPROCEXT(void, UniformMatrix2x4fvNV, (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value), PFNGLUNIFORMMATRIX2X4FVNVPROC, EXTNAME)
GLAPIPROCEXT(void, UniformMatrix4x2fvNV, (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value), PFNGLUNIFORMMATRIX4X2FVNVPROC, EXTNAME)
GLAPIPROCEXT(void, UniformMatrix3x4fvNV, (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value), PFNGLUNIFORMMATRIX3X4FVNVPROC, EXTNAME)
GLAPIPROCEXT(void, UniformMatrix4x3fvNV, (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value), PFNGLUNIFORMMATRIX4X3FVNVPROC, EXTNAME)

#undef EXTNAME /* GL_NV_non_square_matrices */
#define EXTNAME "GL_NV_path_rendering"
GLAPIFUNCEXT(GLuint, GenPathsNV, (GLsizei range), PFNGLGENPATHSNVPROC, EXTNAME)
GLAPIPROCEXT(void, DeletePathsNV, (GLuint path, GLsizei range), PFNGLDELETEPATHSNVPROC, EXTNAME)
GLAPIFUNCEXT(GLboolean, IsPathNV, (GLuint path), PFNGLISPATHNVPROC, EXTNAME)
GLAPIPROCEXT(void, PathCommandsNV, (GLuint path, GLsizei numCommands, const GLubyte *commands, GLsizei numCoords, GLenum coordType, const void *coords), PFNGLPATHCOMMANDSNVPROC, EXTNAME)
GLAPIPROCEXT(void, PathCoordsNV, (GLuint path, GLsizei numCoords, GLenum coordType, const void *coords), PFNGLPATHCOORDSNVPROC, EXTNAME)
GLAPIPROCEXT(void, PathSubCommandsNV, (GLuint path, GLsizei commandStart, GLsizei commandsToDelete, GLsizei numCommands, const GLubyte *commands, GLsizei numCoords, GLenum coordType, const void *coords), PFNGLPATHSUBCOMMANDSNVPROC, EXTNAME)
GLAPIPROCEXT(void, PathSubCoordsNV, (GLuint path, GLsizei coordStart, GLsizei numCoords, GLenum coordType, const void *coords), PFNGLPATHSUBCOORDSNVPROC, EXTNAME)
GLAPIPROCEXT(void, PathStringNV, (GLuint path, GLenum format, GLsizei length, const void *pathString), PFNGLPATHSTRINGNVPROC, EXTNAME)
GLAPIPROCEXT(void, PathGlyphsNV, (GLuint firstPathName, GLenum fontTarget, const void *fontName, GLbitfield fontStyle, GLsizei numGlyphs, GLenum type, const void *charcodes, GLenum handleMissingGlyphs, GLuint pathParameterTemplate, GLfloat emScale), PFNGLPATHGLYPHSNVPROC, EXTNAME)
GLAPIPROCEXT(void, PathGlyphRangeNV, (GLuint firstPathName, GLenum fontTarget, const void *fontName, GLbitfield fontStyle, GLuint firstGlyph, GLsizei numGlyphs, GLenum handleMissingGlyphs, GLuint pathParameterTemplate, GLfloat emScale), PFNGLPATHGLYPHRANGENVPROC, EXTNAME)
GLAPIPROCEXT(void, WeightPathsNV, (GLuint resultPath, GLsizei numPaths, const GLuint *paths, const GLfloat *weights), PFNGLWEIGHTPATHSNVPROC, EXTNAME)
GLAPIPROCEXT(void, CopyPathNV, (GLuint resultPath, GLuint srcPath), PFNGLCOPYPATHNVPROC, EXTNAME)
GLAPIPROCEXT(void, InterpolatePathsNV, (GLuint resultPath, GLuint pathA, GLuint pathB, GLfloat weight), PFNGLINTERPOLATEPATHSNVPROC, EXTNAME)
GLAPIPROCEXT(void, TransformPathNV, (GLuint resultPath, GLuint srcPath, GLenum transformType, const GLfloat *transformValues), PFNGLTRANSFORMPATHNVPROC, EXTNAME)
GLAPIPROCEXT(void, PathParameterivNV, (GLuint path, GLenum pname, const GLint *value), PFNGLPATHPARAMETERIVNVPROC, EXTNAME)
GLAPIPROCEXT(void, PathParameteriNV, (GLuint path, GLenum pname, GLint value), PFNGLPATHPARAMETERINVPROC, EXTNAME)
GLAPIPROCEXT(void, PathParameterfvNV, (GLuint path, GLenum pname, const GLfloat *value), PFNGLPATHPARAMETERFVNVPROC, EXTNAME)
GLAPIPROCEXT(void, PathParameterfNV, (GLuint path, GLenum pname, GLfloat value), PFNGLPATHPARAMETERFNVPROC, EXTNAME)
GLAPIPROCEXT(void, PathDashArrayNV, (GLuint path, GLsizei dashCount, const GLfloat *dashArray), PFNGLPATHDASHARRAYNVPROC, EXTNAME)
GLAPIPROCEXT(void, PathStencilFuncNV, (GLenum func, GLint ref, GLuint mask), PFNGLPATHSTENCILFUNCNVPROC, EXTNAME)
GLAPIPROCEXT(void, PathStencilDepthOffsetNV, (GLfloat factor, GLfloat units), PFNGLPATHSTENCILDEPTHOFFSETNVPROC, EXTNAME)
GLAPIPROCEXT(void, StencilFillPathNV, (GLuint path, GLenum fillMode, GLuint mask), PFNGLSTENCILFILLPATHNVPROC, EXTNAME)
GLAPIPROCEXT(void, StencilStrokePathNV, (GLuint path, GLint reference, GLuint mask), PFNGLSTENCILSTROKEPATHNVPROC, EXTNAME)
GLAPIPROCEXT(void, StencilFillPathInstancedNV, (GLsizei numPaths, GLenum pathNameType, const void *paths, GLuint pathBase, GLenum fillMode, GLuint mask, GLenum transformType, const GLfloat *transformValues), PFNGLSTENCILFILLPATHINSTANCEDNVPROC, EXTNAME)
GLAPIPROCEXT(void, StencilStrokePathInstancedNV, (GLsizei numPaths, GLenum pathNameType, const void *paths, GLuint pathBase, GLint reference, GLuint mask, GLenum transformType, const GLfloat *transformValues), PFNGLSTENCILSTROKEPATHINSTANCEDNVPROC, EXTNAME)
GLAPIPROCEXT(void, PathCoverDepthFuncNV, (GLenum func), PFNGLPATHCOVERDEPTHFUNCNVPROC, EXTNAME)
GLAPIPROCEXT(void, CoverFillPathNV, (GLuint path, GLenum coverMode), PFNGLCOVERFILLPATHNVPROC, EXTNAME)
GLAPIPROCEXT(void, CoverStrokePathNV, (GLuint path, GLenum coverMode), PFNGLCOVERSTROKEPATHNVPROC, EXTNAME)
GLAPIPROCEXT(void, CoverFillPathInstancedNV, (GLsizei numPaths, GLenum pathNameType, const void *paths, GLuint pathBase, GLenum coverMode, GLenum transformType, const GLfloat *transformValues), PFNGLCOVERFILLPATHINSTANCEDNVPROC, EXTNAME)
GLAPIPROCEXT(void, CoverStrokePathInstancedNV, (GLsizei numPaths, GLenum pathNameType, const void *paths, GLuint pathBase, GLenum coverMode, GLenum transformType, const GLfloat *transformValues), PFNGLCOVERSTROKEPATHINSTANCEDNVPROC, EXTNAME)
GLAPIPROCEXT(void, GetPathParameterivNV, (GLuint path, GLenum pname, GLint *value), PFNGLGETPATHPARAMETERIVNVPROC, EXTNAME)
GLAPIPROCEXT(void, GetPathParameterfvNV, (GLuint path, GLenum pname, GLfloat *value), PFNGLGETPATHPARAMETERFVNVPROC, EXTNAME)
GLAPIPROCEXT(void, GetPathCommandsNV, (GLuint path, GLubyte *commands), PFNGLGETPATHCOMMANDSNVPROC, EXTNAME)
GLAPIPROCEXT(void, GetPathCoordsNV, (GLuint path, GLfloat *coords), PFNGLGETPATHCOORDSNVPROC, EXTNAME)
GLAPIPROCEXT(void, GetPathDashArrayNV, (GLuint path, GLfloat *dashArray), PFNGLGETPATHDASHARRAYNVPROC, EXTNAME)
GLAPIPROCEXT(void, GetPathMetricsNV, (GLbitfield metricQueryMask, GLsizei numPaths, GLenum pathNameType, const void *paths, GLuint pathBase, GLsizei stride, GLfloat *metrics), PFNGLGETPATHMETRICSNVPROC, EXTNAME)
GLAPIPROCEXT(void, GetPathMetricRangeNV, (GLbitfield metricQueryMask, GLuint firstPathName, GLsizei numPaths, GLsizei stride, GLfloat *metrics), PFNGLGETPATHMETRICRANGENVPROC, EXTNAME)
GLAPIPROCEXT(void, GetPathSpacingNV, (GLenum pathListMode, GLsizei numPaths, GLenum pathNameType, const void *paths, GLuint pathBase, GLfloat advanceScale, GLfloat kerningScale, GLenum transformType, GLfloat *returnedSpacing), PFNGLGETPATHSPACINGNVPROC, EXTNAME)
GLAPIFUNCEXT(GLboolean, IsPointInFillPathNV, (GLuint path, GLuint mask, GLfloat x, GLfloat y), PFNGLISPOINTINFILLPATHNVPROC, EXTNAME)
GLAPIFUNCEXT(GLboolean, IsPointInStrokePathNV, (GLuint path, GLfloat x, GLfloat y), PFNGLISPOINTINSTROKEPATHNVPROC, EXTNAME)
GLAPIFUNCEXT(GLfloat, GetPathLengthNV, (GLuint path, GLsizei startSegment, GLsizei numSegments), PFNGLGETPATHLENGTHNVPROC, EXTNAME)
GLAPIFUNCEXT(GLboolean, PointAlongPathNV, (GLuint path, GLsizei startSegment, GLsizei numSegments, GLfloat distance, GLfloat *x, GLfloat *y, GLfloat *tangentX, GLfloat *tangentY), PFNGLPOINTALONGPATHNVPROC, EXTNAME)
GLAPIPROCEXT(void, MatrixLoad3x2fNV, (GLenum matrixMode, const GLfloat *m), PFNGLMATRIXLOAD3X2FNVPROC, EXTNAME)
GLAPIPROCEXT(void, MatrixLoad3x3fNV, (GLenum matrixMode, const GLfloat *m), PFNGLMATRIXLOAD3X3FNVPROC, EXTNAME)
GLAPIPROCEXT(void, MatrixLoadTranspose3x3fNV, (GLenum matrixMode, const GLfloat *m), PFNGLMATRIXLOADTRANSPOSE3X3FNVPROC, EXTNAME)
GLAPIPROCEXT(void, MatrixMult3x2fNV, (GLenum matrixMode, const GLfloat *m), PFNGLMATRIXMULT3X2FNVPROC, EXTNAME)
GLAPIPROCEXT(void, MatrixMult3x3fNV, (GLenum matrixMode, const GLfloat *m), PFNGLMATRIXMULT3X3FNVPROC, EXTNAME)
GLAPIPROCEXT(void, MatrixMultTranspose3x3fNV, (GLenum matrixMode, const GLfloat *m), PFNGLMATRIXMULTTRANSPOSE3X3FNVPROC, EXTNAME)
GLAPIPROCEXT(void, StencilThenCoverFillPathNV, (GLuint path, GLenum fillMode, GLuint mask, GLenum coverMode), PFNGLSTENCILTHENCOVERFILLPATHNVPROC, EXTNAME)
GLAPIPROCEXT(void, StencilThenCoverStrokePathNV, (GLuint path, GLint reference, GLuint mask, GLenum coverMode), PFNGLSTENCILTHENCOVERSTROKEPATHNVPROC, EXTNAME)
GLAPIPROCEXT(void, StencilThenCoverFillPathInstancedNV, (GLsizei numPaths, GLenum pathNameType, const void *paths, GLuint pathBase, GLenum fillMode, GLuint mask, GLenum coverMode, GLenum transformType, const GLfloat *transformValues), PFNGLSTENCILTHENCOVERFILLPATHINSTANCEDNVPROC, EXTNAME)
GLAPIPROCEXT(void, StencilThenCoverStrokePathInstancedNV, (GLsizei numPaths, GLenum pathNameType, const void *paths, GLuint pathBase, GLint reference, GLuint mask, GLenum coverMode, GLenum transformType, const GLfloat *transformValues), PFNGLSTENCILTHENCOVERSTROKEPATHINSTANCEDNVPROC, EXTNAME)
GLAPIFUNCEXT(GLenum, PathGlyphIndexRangeNV, (GLenum fontTarget, const void *fontName, GLbitfield fontStyle, GLuint pathParameterTemplate, GLfloat emScale, GLuint baseAndCount[2]), PFNGLPATHGLYPHINDEXRANGENVPROC, EXTNAME)
GLAPIFUNCEXT(GLenum, PathGlyphIndexArrayNV, (GLuint firstPathName, GLenum fontTarget, const void *fontName, GLbitfield fontStyle, GLuint firstGlyphIndex, GLsizei numGlyphs, GLuint pathParameterTemplate, GLfloat emScale), PFNGLPATHGLYPHINDEXARRAYNVPROC, EXTNAME)
GLAPIFUNCEXT(GLenum, PathMemoryGlyphIndexArrayNV, (GLuint firstPathName, GLenum fontTarget, GLsizeiptr fontSize, const void *fontData, GLsizei faceIndex, GLuint firstGlyphIndex, GLsizei numGlyphs, GLuint pathParameterTemplate, GLfloat emScale), PFNGLPATHMEMORYGLYPHINDEXARRAYNVPROC, EXTNAME)
GLAPIPROCEXT(void, ProgramPathFragmentInputGenNV, (GLuint program, GLint location, GLenum genMode, GLint components, const GLfloat *coeffs), PFNGLPROGRAMPATHFRAGMENTINPUTGENNVPROC, EXTNAME)
GLAPIPROCEXT(void, GetProgramResourcefvNV, (GLuint program, GLenum programInterface, GLuint index, GLsizei propCount, const GLenum *props, GLsizei bufSize, GLsizei *length, GLfloat *params), PFNGLGETPROGRAMRESOURCEFVNVPROC, EXTNAME)

#undef EXTNAME /* GL_NV_path_rendering */
#define EXTNAME "GL_NV_polygon_mode"
GLAPIPROCEXT(void, PolygonModeNV, (GLenum face, GLenum mode), PFNGLPOLYGONMODENVPROC, EXTNAME)

#undef EXTNAME /* GL_NV_polygon_mode */
#define EXTNAME "GL_NV_read_buffer"
GLAPIPROCEXT(void, ReadBufferNV, (GLenum mode), PFNGLREADBUFFERNVPROC, EXTNAME)

#undef EXTNAME /* GL_NV_read_buffer */
#define EXTNAME "GL_NV_sample_locations"
GLAPIPROCEXT(void, FramebufferSampleLocationsfvNV, (GLenum target, GLuint start, GLsizei count, const GLfloat *v), PFNGLFRAMEBUFFERSAMPLELOCATIONSFVNVPROC, EXTNAME)
GLAPIPROCEXT(void, NamedFramebufferSampleLocationsfvNV, (GLuint framebuffer, GLuint start, GLsizei count, const GLfloat *v), PFNGLNAMEDFRAMEBUFFERSAMPLELOCATIONSFVNVPROC, EXTNAME)
GLAPIPROCEXT(void, ResolveDepthValuesNV, (void), PFNGLRESOLVEDEPTHVALUESNVPROC, EXTNAME)

#undef EXTNAME /* GL_NV_sample_locations */
#define EXTNAME "GL_NV_viewport_array"
GLAPIPROCEXT(void, ViewportArrayvNV, (GLuint first, GLsizei count, const GLfloat *v), PFNGLVIEWPORTARRAYVNVPROC, EXTNAME)
GLAPIPROCEXT(void, ViewportIndexedfNV, (GLuint index, GLfloat x, GLfloat y, GLfloat w, GLfloat h), PFNGLVIEWPORTINDEXEDFNVPROC, EXTNAME)
GLAPIPROCEXT(void, ViewportIndexedfvNV, (GLuint index, const GLfloat *v), PFNGLVIEWPORTINDEXEDFVNVPROC, EXTNAME)
GLAPIPROCEXT(void, ScissorArrayvNV, (GLuint first, GLsizei count, const GLint *v), PFNGLSCISSORARRAYVNVPROC, EXTNAME)
GLAPIPROCEXT(void, ScissorIndexedNV, (GLuint index, GLint left, GLint bottom, GLsizei width, GLsizei height), PFNGLSCISSORINDEXEDNVPROC, EXTNAME)
GLAPIPROCEXT(void, ScissorIndexedvNV, (GLuint index, const GLint *v), PFNGLSCISSORINDEXEDVNVPROC, EXTNAME)
GLAPIPROCEXT(void, DepthRangeArrayfvNV, (GLuint first, GLsizei count, const GLfloat *v), PFNGLDEPTHRANGEARRAYFVNVPROC, EXTNAME)
GLAPIPROCEXT(void, DepthRangeIndexedfNV, (GLuint index, GLfloat n, GLfloat f), PFNGLDEPTHRANGEINDEXEDFNVPROC, EXTNAME)
GLAPIPROCEXT(void, GetFloati_vNV, (GLenum target, GLuint index, GLfloat *data), PFNGLGETFLOATI_VNVPROC, EXTNAME)
GLAPIPROCEXT(void, EnableiNV, (GLenum target, GLuint index), PFNGLENABLEINVPROC, EXTNAME)
GLAPIPROCEXT(void, DisableiNV, (GLenum target, GLuint index), PFNGLDISABLEINVPROC, EXTNAME)
GLAPIFUNCEXT(GLboolean, IsEnablediNV, (GLenum target, GLuint index), PFNGLISENABLEDINVPROC, EXTNAME)

#undef EXTNAME /* GL_NV_viewport_array */
#define EXTNAME "GL_OVR_multiview"
GLAPIPROCEXT(void, FramebufferTextureMultiviewOVR, (GLenum target, GLenum attachment, GLuint texture, GLint level, GLint baseViewIndex, GLsizei numViews), PFNGLFRAMEBUFFERTEXTUREMULTIVIEWOVRPROC, EXTNAME)

#undef EXTNAME /* GL_OVR_multiview */
#define EXTNAME "GL_OVR_multiview_multisampled_render_to_texture"
GLAPIPROCEXT(void, FramebufferTextureMultisampleMultiviewOVR, (GLenum target, GLenum attachment, GLuint texture, GLint level, GLsizei samples, GLint baseViewIndex, GLsizei numViews), PFNGLFRAMEBUFFERTEXTUREMULTISAMPLEMULTIVIEWOVRPROC, EXTNAME)

#undef EXTNAME /* GL_OVR_multiview_multisampled_render_to_texture */
#define EXTNAME "GL_QCOM_alpha_test"
GLAPIPROCEXT(void, AlphaFuncQCOM, (GLenum func, GLclampf ref), PFNGLALPHAFUNCQCOMPROC, EXTNAME)

#undef EXTNAME /* GL_QCOM_alpha_test */
#define EXTNAME "GL_QCOM_driver_control"
GLAPIPROCEXT(void, GetDriverControlsQCOM, (GLint *num, GLsizei size, GLuint *driverControls), PFNGLGETDRIVERCONTROLSQCOMPROC, EXTNAME)
GLAPIPROCEXT(void, GetDriverControlStringQCOM, (GLuint driverControl, GLsizei bufSize, GLsizei *length, GLchar *driverControlString), PFNGLGETDRIVERCONTROLSTRINGQCOMPROC, EXTNAME)
GLAPIPROCEXT(void, EnableDriverControlQCOM, (GLuint driverControl), PFNGLENABLEDRIVERCONTROLQCOMPROC, EXTNAME)
GLAPIPROCEXT(void, DisableDriverControlQCOM, (GLuint driverControl), PFNGLDISABLEDRIVERCONTROLQCOMPROC, EXTNAME)

#undef EXTNAME /* GL_QCOM_driver_control */
#define EXTNAME "GL_QCOM_extended_get"
GLAPIPROCEXT(void, ExtGetTexturesQCOM, (GLuint *textures, GLint maxTextures, GLint *numTextures), PFNGLEXTGETTEXTURESQCOMPROC, EXTNAME)
GLAPIPROCEXT(void, ExtGetBuffersQCOM, (GLuint *buffers, GLint maxBuffers, GLint *numBuffers), PFNGLEXTGETBUFFERSQCOMPROC, EXTNAME)
GLAPIPROCEXT(void, ExtGetRenderbuffersQCOM, (GLuint *renderbuffers, GLint maxRenderbuffers, GLint *numRenderbuffers), PFNGLEXTGETRENDERBUFFERSQCOMPROC, EXTNAME)
GLAPIPROCEXT(void, ExtGetFramebuffersQCOM, (GLuint *framebuffers, GLint maxFramebuffers, GLint *numFramebuffers), PFNGLEXTGETFRAMEBUFFERSQCOMPROC, EXTNAME)
GLAPIPROCEXT(void, ExtGetTexLevelParameterivQCOM, (GLuint texture, GLenum face, GLint level, GLenum pname, GLint *params), PFNGLEXTGETTEXLEVELPARAMETERIVQCOMPROC, EXTNAME)
GLAPIPROCEXT(void, ExtTexObjectStateOverrideiQCOM, (GLenum target, GLenum pname, GLint param), PFNGLEXTTEXOBJECTSTATEOVERRIDEIQCOMPROC, EXTNAME)
GLAPIPROCEXT(void, ExtGetTexSubImageQCOM, (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, void *texels), PFNGLEXTGETTEXSUBIMAGEQCOMPROC, EXTNAME)
GLAPIPROCEXT(void, ExtGetBufferPointervQCOM, (GLenum target, void **params), PFNGLEXTGETBUFFERPOINTERVQCOMPROC, EXTNAME)

#undef EXTNAME /* GL_QCOM_extended_get */
#define EXTNAME "GL_QCOM_extended_get2"
GLAPIPROCEXT(void, ExtGetShadersQCOM, (GLuint *shaders, GLint maxShaders, GLint *numShaders), PFNGLEXTGETSHADERSQCOMPROC, EXTNAME)
GLAPIPROCEXT(void, ExtGetProgramsQCOM, (GLuint *programs, GLint maxPrograms, GLint *numPrograms), PFNGLEXTGETPROGRAMSQCOMPROC, EXTNAME)
GLAPIFUNCEXT(GLboolean, ExtIsProgramBinaryQCOM, (GLuint program), PFNGLEXTISPROGRAMBINARYQCOMPROC, EXTNAME)
GLAPIPROCEXT(void, ExtGetProgramBinarySourceQCOM, (GLuint program, GLenum shadertype, GLchar *source, GLint *length), PFNGLEXTGETPROGRAMBINARYSOURCEQCOMPROC, EXTNAME)

#undef EXTNAME /* GL_QCOM_extended_get2 */
#define EXTNAME "GL_QCOM_tiled_rendering"
GLAPIPROCEXT(void, StartTilingQCOM, (GLuint x, GLuint y, GLuint width, GLuint height, GLbitfield preserveMask), PFNGLSTARTTILINGQCOMPROC, EXTNAME)
GLAPIPROCEXT(void, EndTilingQCOM, (GLbitfield preserveMask), PFNGLENDTILINGQCOMPROC, EXTNAME)

#undef EXTNAME /* GL_QCOM_tiled_rendering */
