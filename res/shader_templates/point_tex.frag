vec3 tex_point_pos = vec3({{ pos.x }}, {{ pos.y }}, {{ pos.z }});

vec3 tex_point_lightDir = normalize(tex_point_pos - worldPos.xyz);

// Diffuse intensity (dot product)
float tex_point_diff = dot(g_norm, tex_point_lightDir);
vec3 tex_point_result;
// Binary lighting
if (tex_point_diff > 0.0)
tex_point_result = vec3(1.0);
else
tex_point_result = vec3(0.0);

    FragColor.rgb = mix(FragColor.rgb, texture(texture1, TexCoords).rgb, tex_point_result);