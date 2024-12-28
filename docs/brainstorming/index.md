
{% for item in site.data.navigationlist.pages %}
{% if item.name == "Brainstorming" %}
    {% continue %}
{% endif %}

{% if item.url %}
    <h3> <a href="{{entry.url}}"> {{ item.name }} </a></h3>
{% else %}
    <h3>{{ item.name }}</h3>
{% endif %}
<ul>
{% for entry in item.pages %}
    <li><a href="{{entry.url}}"> {{entry.name}} </a></li>
{% endfor %}
</ul>
{% endfor %}