# html-parser
Example:

```
bool list_org_render(int org_length, Value &value, val layer)
{
    vector<string> org_name;
    for (i = 0; i < org_length; i++)
    {
        org_name.push_back(org_list["resources"][i]["name"].GetString());
    };

    sort(org_name.begin(), org_name.end(), no_case);

    for (i = 0; i < org_length; i++)
    {
        render(layer, org_list_template, (Value &)org_list_template, (char *)org_name[i].c_str(), true, img);
    };

    val::global("document").call<val>("getElementById", string("org_list")).call<val>("appendChild", layer);

    return true;
}

void list_org(unsigned handle, void *userData, void *data, unsigned size)
{
    val layer = val::global("document").call<val>("createDocumentFragment");
    org_list.Parse<rapidjson::kParseStopWhenDoneFlag>((char *)data);
    SizeType org_length = org_list["total"].GetInt();

    if (list_org_render(org_length, (Value &)org_list_template, layer))
    {
        emscripten_async_wget2_data("mp/v1/org_quotas", "GET", "", NULL, 1, list_org_quota, onerror, onprogress);
        for (i = 0; i < org_length; i++)
        {
            string url = "mp/v1/orgs/";
            url.append(org_list["resources"][i]["guid"].GetString()).append("/usage");
            emscripten_async_wget2_data(url.c_str(), "GET", "", (void *)org_list["resources"][i]["name"].GetString(), 1, list_usage, onerror, onprogress);
        };
    };
}

void load_org_list_template(unsigned handle, void *userData, void *data, unsigned size)
{
    org_list_template.SetObject();
    org_list_template.Parse<rapidjson::kParseStopWhenDoneFlag>((char *)data);
}

int main()
{
    emscripten_async_wget2_data("./org_list.json", "GET", NULL, NULL, 1, load_org_list_template, onerror, onprogress);
    return 0;
}
```
