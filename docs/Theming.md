# vcv-rackthemer
Themes are defined in json files included with your plugin's resources. You can have as many themes as you like, and different themes can be used simultaneously.  
A theme is a collection of styles, similar to a very simplified CSS. Styles can address a limited number of attributes to reflect the theme.

## SVG element ids and style tags
The SVG element id is used to target an element for theming.  
Inkscape preserves ids across edits of the SVG, so the mechanism is robust to editing.  
The library supports theming by "id" or by "class". Each element can only have one class, and classes are added by appending the class name to the id, preceded by double dashes. For example:

```xml
<circle id="outside_ring--theme-bezel" cx="7.5" cy="7.5" r="6.2" fill="#999999" />
```
Styles targetting ids take precedence over styles targetting classes. If an element is targetted by both a class and an id style, the id style will be applied over the class style.


In this example, all the tags begin with `theme-`, but this prefix isn't required.

You can edit an object's id in Inkscape: Right click and choose **Object Properties...**.  
The id is separate from Inkscape's object label which is kept in a separate `inkscape:label` attribute.  
Inkscape uses its labels for what it shows you in **Layers and Objects** if present. If the `inkscape:label` attribute isn't present, it'll use the element's id.  

Nano restricts ids to a maximum of 63 characters.

> Note: Unfortunately, the current Rack SVGs in the component library do not contain ids, so they cannot be themed.  
If you want to target Rack's SVG widgets, you must subclass the Rack widget and supply your own SVG(s) with ids added.

## Theme JSON format
The JSON is an object containing a name and an array of styles.

A color is specified as a json string starting with a pound sign (often called 'hash') #, followed by the hexadecimal code. Syntax is #RGB, #RGBA (single hex digit for each component), or #RRGGBB, #RRGGBBAA (two hex digits for each component). This hex notation is shown by the color pickers in most graphical editing applications, online color pickers, etc.

When creating your default theme, you can copy the colors directly from the SVG using a text editor or Inkscape.

A limited set of attributes can be modified by theming:

- Opacity of the element. A floating-point value from 0 to 1.0.

- Stroke and fill color. Unlike SVG, colors can include opacity.
To create transparent colors, append an alpha component to the RGB.
For example, this color is a 50% transparent medium blue: `#4086bf80`, where the `80` is the alpha value.

  Targeting only the opacity of a stroke or fill color is not supported -- only the full color as one unit.

- Stroke width. A floating point value for the width fo a stroke.

- Stroke line cap. A string specifying which kind of cap to use. The following types are supported:
    - `butt`
    - `round`
    - `square`
    - `bevel`
    - `miter`

- Color and offset of an existing gradient stop.

To target a gradient, the element in the SVG must also define a gradient with a stop at that index (0 or 1).
This is necessary because the scheme does not have a complete definition of a gradient,
and we don't have the ability to create and destroy gradients trivially due to transforms.

- Stroke dashes are not supported, but there are no barriers to implementing support for it.

## Example theme
```json
{
    "name": "Light",
    "theme": {
        "theme_background": {
            "fill": {
                "gradient": [
                    { "index": 0, "color": "#ee2e63", }
                ]
            },

            "stroke": { "color": "#808080", "width": 1.5 }
        },

        "theme_no-fill": {
            "fill": "none"
        },

        "theme_no-stroke": {
            "stroke": "none"
        },

        "theme_bezel": {
            "fill": "#808080",
            "stroke": { "color": "#323232", "line_cap"
        },

        ".screw-socket": {
            "fill": "#3d3d3d",
            "stroke": "#1a1a1a"
        }
    }
}
```

Each style can have **opacity**, **fill**, and **stroke**.

### opacity

Sets the opacity of the entire element. This is a floating-point value between 0 and 1.0.

### fill

The **fill** is a hex color string value, **none**, or an object containing a **gradient**:

```json
    "strokeless-purple": {
        "fill" : "#952a9ccc",
        "stroke": "none"
    }
```

A gradient cannot be applied to an element that does not have one.

Here is an example of a full gradient between two opaque colors:

```json
    "budgie": {
        "fill": {
            "gradient": [
                { "index": 0, "color": "#3f7837" },
                { "index": 1, "color": "#a01466" }
            ]
        }
    }
```

In a gradient, **index** is required to know whether to change gradient stop 0 or stop 1.
**color** and **offset** are optional, and need to be present only if you're changing it.

### stroke

A **stroke** is a hex color string value, **none**, or an object containing optional **color** string, **width** number, **line_cap** string, and **gradient** object.

The **gradient** object for stroke is identical to a gradient for a fill.

## Creating a theme

- Start with a design that will be one of your themes.
This will be your default theme and if you follow the Rack model, it would be your 'Light' theme.

  For the rest of this discussion we'll refer to this as the "Light" theme.

- Decide which elements will change depending on the theme.

- For each theme-able element, define a style in your Light theme in the JSON to address the things that will be changed.

- Copy the Light theme, rename it, and change the colors to suit.