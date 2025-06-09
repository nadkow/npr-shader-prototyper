    //get the dot product between the normal and the view direction
    vec3 fresnel_viewDir_{{ node_id }} = normalize(viewPos - vec3(worldPos));
    float fresnel_{{ node_id }} = dot(Normal, fresnel_viewDir_{{ node_id }});
    //invert the fresnel so the big values are on the outside
    fresnel_{{ node_id }} = clamp(fresnel_{{ node_id }}, 0.0, 1.0);
    //hardcoded multiplication blend mode
    FragColor.xyz = mix(vec3({{ color.r }}, {{ color.g }}, {{ color.b }}), FragColor.xyz, fresnel_{{ node_id }});