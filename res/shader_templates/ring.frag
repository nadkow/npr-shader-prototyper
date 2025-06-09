    //get the dot product between the normal and the view direction
    vec3 ring_viewDir_{{ node_id }} = normalize(viewPos - vec3(worldPos));
    float ring_dotproduct_{{ node_id }} = max(dot(Normal, ring_viewDir_{{ node_id }}), .3);
    float ring_value_{{ node_id }} = sin({{ bands }}/ring_dotproduct_{{ node_id }}) * .5 + .5;
    FragColor.xyz = vec3(ring_value_{{ node_id }}, ring_value_{{ node_id }}, ring_value_{{ node_id }});