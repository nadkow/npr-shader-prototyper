    vec3 tex_point_pos_{{ node_id }} = vec3({{ pos.x }}, {{ pos.y }}, {{ pos.z }});
    vec3 tex_point_lightDir_{{ node_id }} = normalize(tex_point_pos_{{ node_id }} - worldPos.xyz);
    // Diffuse intensity (dot product)
    float tex_point_diff_{{ node_id }} = dot(g_norm, tex_point_lightDir_{{ node_id }});
    vec3 tex_point_result_{{ node_id }};
    // Binary lighting
    if (tex_point_diff_{{ node_id }} > 0.0)
    tex_point_result_{{ node_id }} = vec3(1.0);
    else
    tex_point_result_{{ node_id }} = vec3(0.0);

    FragColor.rgb = mix(FragColor.rgb, texture(texture1, TexCoords).rgb, tex_point_result_{{ node_id }});