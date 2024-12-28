# NOTE: As of now, I didn't put the time in to get these mermaid charts rendering in Jekyll. To view the charts, look at the markdown files in github.
## Navigation

{% for item in site.data.navigationlist.pages %}
<h3>{{ item.name }}</h3>
<ul>
{% for entry in item.pages %}
<li><a href="{{entry.url}}"> {{entry.name}} </a></li>
{% endfor %}
</ul>
{% endfor %}

## Section Descriptions
### Brainstorming

This section is where I put more unstructured thoughts before they get promoted to a real section
or prior to writing a real draft. Mostly a way to organize my thoughts in a more permanent fashion.
