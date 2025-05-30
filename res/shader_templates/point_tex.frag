vec3 tex_point_pos = vec3({{ pos.x }}, {{ pos.y }}, {{ pos.z }});

    float tex_point_constant = 1.0f;
    float tex_point_linear = 0.35f;
    float tex_point_quadratic = 0.44f;
    float tex_point_distance = length(tex_point_pos - worldPos.xyz);
    float tex_point_attenuation = 1.0 / (tex_point_constant + tex_point_linear * tex_point_distance + tex_point_quadratic * (tex_point_distance * tex_point_distance));

    vec3 tex_pointDir = normalize(tex_point_pos - worldPos.xyz);
    float tex_pointDiff = max(dot(g_norm, tex_pointDir), 0.0);

    vec3 tex_point_result = (tex_pointDiff * vec3(1.,1.,1.)) * tex_point_attenuation;

    FragColor.rgb = mix(FragColor.rgb, texture(texture1, TexCoords).rgb, tex_point_result);