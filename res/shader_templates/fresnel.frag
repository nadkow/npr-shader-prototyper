 //get the dot product between the normal and the view direction
 vec3 fresnel_viewDir = normalize(viewPos - vec3(worldPos));
 float fresnel = dot(Normal, fresnel_viewDir);
 //invert the fresnel so the big values are on the outside
 fresnel = clamp(fresnel, 0.0, 1.0);
 //hardcoded multiplication blend mode
 FragColor.xyz = mix(vec3({{ color.r }}, {{ color.g }}, {{ color.b }}), FragColor.xyz, fresnel);