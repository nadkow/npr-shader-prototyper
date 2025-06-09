    vec3 point_pos_{{ node_id }} = vec3({{ pos.x }}, {{ pos.y }}, {{ pos.z }});
    vec3 point_color_{{ node_id }} = vec3({{ color.r }}, {{ color.g }}, {{ color.b }});

    float point_constant_{{ node_id }} = 1.0f;
    float point_linear_{{ node_id }} = 0.35f;
    float point_quadratic_{{ node_id }} = 0.44f;
    float point_distance_{{ node_id }} = length(point_pos_{{ node_id }} - worldPos.xyz);
    float point_attenuation_{{ node_id }} = 1.0 / (point_constant_{{ node_id }} + point_linear_{{ node_id }} * point_distance_{{ node_id }} + point_quadratic_{{ node_id }} * (point_distance_{{ node_id }} * point_distance_{{ node_id }}));

    vec3 pointDir_{{ node_id }} = normalize(point_pos_{{ node_id }} - worldPos.xyz);
    float pointDiff_{{ node_id }} = max(dot(g_norm, pointDir_{{ node_id }}), 0.0);

    vec3 point_result_{{ node_id }} = (pointDiff_{{ node_id }} * vec3(1.,1.,1.)) * point_attenuation_{{ node_id }};

    FragColor.rgb = mix(FragColor.rgb, point_color_{{ node_id }}, point_result_{{ node_id }});