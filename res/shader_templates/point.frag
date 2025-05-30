vec3 point_pos = vec3({{ pos.x }}, {{ pos.y }}, {{ pos.z }});
vec3 point_color = vec3({{ color.r }}, {{ color.g }}, {{ color.b }});

    float point_constant = 1.0f;
    float point_linear = 0.35f;
    float point_quadratic = 0.44f;
    float point_distance = length(point_pos - worldPos.xyz);
    float point_attenuation = 1.0 / (point_constant + point_linear * point_distance + point_quadratic * (point_distance * point_distance));

    vec3 pointDir = normalize(point_pos - worldPos.xyz);
    float pointDiff = max(dot(g_norm, pointDir), 0.0);

    vec3 point_result = (pointDiff * vec3(1.,1.,1.)) * point_attenuation;

    FragColor.rgb = mix(FragColor.rgb, point_color, point_result);