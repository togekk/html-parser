using namespace std;
using namespace rapidjson;
using namespace emscripten;

val render_node(val parent, char *type, Value &attr, char *text, int number, float decimal, char unit, Document &img)
{
    val layer_return = val::object();
    if (type != NULL)
    {
        val layer_new = val::global("document").call<val>("createDocumentFragment");
        val layer_sub = val::global("document").call<val>("createElement", string(type));

        if (!attr.IsNull())
        {
            for (Value::ConstMemberIterator M = attr.MemberBegin(); M != attr.MemberEnd(); M++)
            {
                string key = M->name.GetString();
                string value = M->value.GetString();
                layer_sub.call<val>("setAttribute", key, value);

                if (key == "ng-src")
                {
                    key = "src";
                    value = img[value.c_str()].GetString();
                    layer_sub.call<val>("setAttribute", key, value);
                }
                if (key == "ng-hide")
                {
                    key = "style";
                    value = "display:none";
                    layer_sub.call<val>("setAttribute", key, value);
                }
            }
        }
        if (text != NULL)
        {
            val textnode_new = val::global("document").call<val>("createTextNode", string(text));
            layer_sub.call<val>("appendChild", textnode_new);
        }
        if (number != NULL)
        {
            val textnode_new = val::global("document").call<val>("createTextNode", number);
            layer_sub.call<val>("appendChild", textnode_new);
        }
        if (decimal != NULL)
        {
            val a = val::object();
            a.set("decimal", decimal);
            val textnode_new = val::global("document").call<val>("createTextNode", a["decimal"].call<val>("toFixed", 1));
            layer_sub.call<val>("appendChild", textnode_new);
        }
        if (unit != NULL)
        {
            val textnode_new = val::global("document").call<val>("createTextNode", string(1, unit));
            layer_sub.call<val>("appendChild", textnode_new);
        }
        layer_new.call<val>("appendChild", layer_sub);
        parent.call<val>("appendChild", layer_new);
        layer_return = layer_sub;
    }
    else
    {
        layer_return = parent;
    }
    return layer_return;
}

void render(val parent, Document &doc, Value &layer, char *name, bool first_layer, Document &img)
{
    if (layer.HasMember("content") && layer["content"].Size() > 0)
    {
        for (SizeType i = 0; i < layer["content"].Size(); i++)
        {

            if (layer["content"][i].IsObject())
            {
                char *type = (char *)layer["content"][i]["type"].GetString();
                char *text;
                string class_name;
                bool has_attr = layer["content"][i].HasMember("attributes");
                if (first_layer && name != NULL)
                {
                    layer["content"][i]["attributes"].AddMember("id", Value(StringRef(name)), doc.GetAllocator());
                }

                if (has_attr && layer["content"][i]["attributes"].HasMember("class"))
                {
                    class_name = layer["content"][i]["attributes"]["class"].GetString();

                    if (class_name == "blue_title col-sm-7")
                    {
                        text = name;
                    }
                    else if (class_name == "instance_title")
                    {
                        text = (char *)"Instance";
                    }
                    else if (class_name == "memory_title margin_top_10")
                        text = (char *)"Memory";
                }

                val layer_this = val::object();

                if (has_attr)
                {
                    if (first_layer)
                    {
                        layer_this = render_node(parent, type, layer["content"][0]["attributes"], text, NULL, NULL, NULL, img);
                    }
                    else
                    {
                        layer_this = render_node(parent, type, layer["content"][i]["attributes"], text, NULL, NULL, NULL, img);
                    }
                }
                else
                {
                    Value empty;
                    layer_this = render_node(parent, type, empty, text, NULL, NULL, NULL, img);
                }

                text = NULL;
                Value &b = layer["content"][i];
                render(layer_this, doc, b, name, false, img);
            }
        }
    }
}

bool no_case(string a, string b)
{
    std::transform(a.begin(), a.end(), a.begin(), ::tolower);
    std::transform(b.begin(), b.end(), b.begin(), ::tolower);
    return (a < b);
}

void check_zero(val layer, float value, Value &attr, char unit, bool is_float)
{
    Document img;
    switch ((int)value)
    {
    case 0:
        render_node(layer, (char *)"div", attr, (char *)"0", NULL, NULL, unit, img);
        break;
    case -1:
        render_node(layer, (char *)"div", attr, (char *)"Unlimited", NULL, NULL, unit, img);
        break;
    default:
    {
        if (is_float)
        {
            render_node(layer, (char *)"div", attr, NULL, NULL, value, unit, img);
        }
        else
        {
            render_node(layer, (char *)"div", attr, NULL, (int)value, NULL, unit, img);
        }
    }
    }
}

void show_unit(val layer, float value, Value &attr)
{
    char unit = 'M';
    bool isMB = true;
    if (value >= 1024)
    {
        value /= 1024;
        unit = 'G';
        isMB = false;
    }
    if (value >= 1024 * 1024)
    {
        value /= 1024;
        unit = 'T';
        isMB = false;
    };

    if (isMB)
    {
        check_zero(layer, value, attr, unit, false);
    }
    else
    {
        Document img;
        render_node(layer, (char *)"div", attr, NULL, NULL, value, unit, img);
    }
}
