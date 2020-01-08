#version 450

/***********************************************************************
*
* Copyright (c) 2019-2020 Barbara Geller
* Copyright (c) 2019-2020 Ansel Sermersheim
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
layout(location = 0) out vec4 outColor;

void main() {
   vec3 Eye       = normalize(-inEyePos);
   vec3 Reflected = normalize(reflect(-inLightVec, inNormal));

   vec4 IAmbient  = vec4(0.4, 0.4, 0.4, 1.0);
   vec4 IDiffuse  = vec4(0.5, 0.5, 0.5, 0.5) * max(dot(inNormal, inLightVec), 0.0);
   float specular = 0.50;
   vec4 ISpecular = vec4(0.9, 0.9, 0.9, 1.0) * pow(max(dot(Reflected, Eye), 0.0), 1.5) * specular;

   outColor = vec4((IAmbient + IDiffuse) * vec4(inColor, 1.0) + ISpecular);
}