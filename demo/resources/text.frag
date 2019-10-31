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
* CsPaint is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
* https://opensource.org/licenses/BSD-2-Clause
*
***********************************************************************/

layout(location = 0) in vec3 inNormal;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inEyePos;
layout(location = 3) in vec3 inLightVec;
layout(location = 4) in vec2 texCoords;
layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D fontSampler;

float median(float r, float g, float b) {
    return max(min(r, g), min(max(r, g), b));
}

const float smoothing = 1.0/64.0;

void main() {

   vec3 fontSample = texture(fontSampler, texCoords).rgb;
   float sigDist = median(fontSample.r, fontSample.g, fontSample.b);
   float opacity = smoothstep(0.5-smoothing, 0.5+smoothing, sigDist);

   if(opacity < 0.05) {
      discard;
   }

   vec3 Eye       = normalize(-inEyePos);
   vec3 Reflected = normalize(reflect(-inLightVec, inNormal));

   vec4 IAmbient  = vec4(0.6, 0.6, 0.6, 1.0);
   vec4 IDiffuse  = vec4(0.5, 0.5, 0.5, 0.5) * max(dot(inNormal, inLightVec), 0.0);
   float specular = 0.50;
   vec4 ISpecular = vec4(0.9, 0.9, 0.9, 1.0) * pow(max(dot(Reflected, Eye), 0.0), 1.5) * specular;

   vec4 tmpColor = vec4((IAmbient + IDiffuse) * vec4(inColor, 1.0) + ISpecular);

   outColor = vec4(tmpColor.rgb, opacity);
}