#version 450

/***********************************************************************
*
* Copyright (c) 2019 Barbara Geller
* Copyright (c) 2019 Ansel Sermersheim
*
* This file is part of CsPaint.
*
* CsPaint is free software, released under the BSD 2-Clause license.
* For license details refer to LICENSE provided with this project.
*
* CopperSpice is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
* https://opensource.org/licenses/BSD-2-Clause
*
***********************************************************************/

layout(location = 0) in vec3 inVertexPosition;
layout(location = 1) in vec3 inVertexNormal;
layout(location = 2) in vec3 inVertexColor;
layout(location = 3) in vec2 inTexCoords;

uniform UniformBufferObject {
  mat4x4 model[6];
  mat4x4 view;
  mat4x4 proj;
  mat4x4 normal[6];
  vec3 lightpos;
} ubo;

layout(location = 0) out vec3 outNormal;
layout(location = 1) out vec3 outVertexColor;
layout(location = 2) out vec3 outEyePos;
layout(location = 3) out vec3 outLightVec;
layout(location = 4) out vec2 outTexCoords;

void main() {
  outNormal      = normalize(mat3(ubo.normal[gl_InstanceIndex]) * inVertexNormal);
  outVertexColor = inVertexColor;
  outTexCoords   = inTexCoords;

  mat4 modelView = ubo.view * ubo.model[gl_InstanceIndex];
  vec4 pos       = modelView * vec4(inVertexPosition, 1);

  outEyePos      = vec3(modelView * vec4(inVertexPosition,1));
  vec4 lightPos  = vec4(ubo.lightpos, 1.0) * ubo.view;

  outLightVec    = normalize(lightPos.xyz - outEyePos);
  gl_Position    = ubo.proj * modelView * vec4(inVertexPosition, 1.0f);
}
