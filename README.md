# Path tracer

This is a path tracer inteded just for learning, it implements diffuse and specular objects with dielectric objects too. 
An example of the output can be found ond `out\expensive.png`.

## How to build

1. Install GNU make for Mac OS or LInux, in Windows mingw can be used.
2. Go to the `build` folder and exectue `make Linux` or `make Windows` 

## How to use it

IF you only want to generate a scene you can use a json file, there is an example of an scene in `assets` folder.

All scenes must contain:

- *width*: the width of the output image.
- *height*: the height of the output image.
- *rays*: the number of rays per pixel.
- *bounces*: the max number of bounces a ray is allowed to give.
- *threads*: the number of threads that will be used, it is recommended that it is set to the number of cores.
- *ouput*: it is an object that contains the path to the output file, it only supports bmp format.
- *camera*: it is an object that defines the camera position orientation and field of view.
- *focus*: it is the point that the camera will look at.
- *ambient color*: is the color sorrounding all the objects, when the ray doesn't hit something it is the default color.
- *objects*: the array of objects that are part of the scene.

```
{
    "width" : 1920,
    "height" : 1080,
    "rays": 10000,
    "bounces": 5,
    "threads" : 10,
    "output" : {
        "path": "../out/my_result.bmp"
    },

    "camera" : {
        "fov" : 45 ,
        "up" : {
            "x" : 0.0,
            "y" : 1.0,
            "z" : 0.0
        },
        "position" : {
            "x" : 0.0,
            "y" : 15.0,
            "z" : -60.0
        }

    },

    "focus" : {
        "x" : 0.0,
        "y" : 0.0,
        "z" : 100.0
    },

    "ambient color" :  {
        "r" : 0,
        "g" : 0,
        "b" : 0
    },

    "objects" : [ ]
```
### Types of basic objects

- *numbers* : Numbers can be floating and integer or it can be a random integer from a minimum boundary to a maximum boundary
```
{
    "number" : 3.14,
    "random number" : {
        "min" : 1.0,
        "max" : 3.0
    }
}

```
- *vectors* : Vector are defined by its x,y and z coordinates it can also be a random vector around an hemisphere for this you have to specify 
the radius of the hemisphere, the spread (180 half sphere, 360 whole sphere), the normal and the center of the hemisphere. All of this numbers 
also accept the random format explained above.
```
{
    "normal vector" : {                
                        "x" : 0.0,
                        "y" : 1.0,
                        "z" : 0.0
                    },
    "random vector" :  {
                "hemisphere" : {
                    "radius": 25.0,
                    "spread" : 180.0,
                    "normal" :  {                
                        "x" : 0.0,
                        "y" : 1.0,
                        "z" : 0.0
                    },
                    "center" : {                
                        "x" : 0.0,
                        "y" : 0.0,
                        "z" : 0.0
                    }
                }
            }               
}

```

- *colors* : Colors are defined by its red, green and blue components from 0 to 255 each one. It also accepts the random number format.

```
{
    "ambient color" :  {
        "r" : 0,
        "g" : 0,
        "b" : 0
    }
}

```

- *materials* : Materials must include a `color`, an `emmitance` attribute if it is above 0.0 it works as a light, `probability of difffuse` is
the probability from 0.0 to 1.0 that a ray is randomly diffused or reflected like a mirror. `angle of spread` only applies when a ray is reflected it diffuses
the reflected ray inside a cone of the given angle. Finally the "refractive index" attribute is optional and it is only used if the material is something like glass.

```
{
    "dielectric material" : {
        "color" : {
            "r" : 255,
            "g" : 255,
            "b" : 255
        },
        "emmitance" : 0.0,
        "probability diffuse" : 0.0,
        "angle of spread" : 10.0,
        "refractive index" : 1.45
    },

    "usual material" : {
        "color" : {
            "r" : 255,
            "g" : 255,
            "b" : 255
        },
        "emmitance" : 0.0,
        "probability diffuse" : 0.0,
        "angle of spread" : 10.0
    },
}

```

- *transformation* : A transformation can include a rotation in any of the axis and translation in any of them, it performs the rotation first and then the translation.

```
{
    "example transformation" : { 
            "rx": 0.0,
            "ry": 180.0,
            "rz": 0.0,
            "tx": 0.0,
            "ty": 0.0,
            "tz": 0.0
        }

}

```

### Types of ray traceable objects

These objects produce an output images, all of this objects must contain the *type* attribute telling the path tracer which kind of object to expect.

- *spheres* : Spheres are dfined by its center and radius. It must contain the *material* attribute which specifies the material of the sphere. 
It can incldue the "repetitions" attribute which repeats the same object an n number of times, it produces interesting results when it is combined with
random values for colors and positions.

```
{
        "usual sphere" : {
        "type" : "sphere",
        "radius" : 5.0,
        "center" :  {           
                    "x" : 0.0,
                    "y" : 10.0,
                    "z" : 0.0
        },
        "material" : {
            "color" : {
                "r" : 255,
                "g" : 255,
                "b" : 255
            },
            "emmitance" : 0.0,
            "probability diffuse" : 0.0,
            "angle of spread" : 10.0
        }
    },

    "bunch of random spheres" :  {
        "type" : "sphere",
        "repeat" : 20,
        "radius" : {"min" :1.0, "max" : 2.0},
        "center" :  {
            "hemisphere" : {
                "radius": 25.0,
                "spread" : 180.0,
                "normal" :  {                
                    "x" : 0.0,
                    "y" : 1.0,
                    "z" : 0.0
                },
                "center" : {                
                    "x" : 0.0,
                    "y" : 0.0,
                    "z" : 0.0
                }
            }
        },
        "material" : {
            "color" : {
                "r" : {"min" : 0.0, "max" : 255.0},
                "g" : {"min" : 0.0, "max" : 255.0},
                "b" : {"min" : 0.0, "max" : 255.0}
            },
            "emmitance" : 0.0,
            "probability diffuse" : {"min" : 0.5, "max" : 1.0},
            "angle of spread" : {"min" : 5.0, "max" : 10.0}
        } 
    }
}

```

- *planes* : Planes are circles and are defined by a point in the plane, and the normal of it which defines the orientation. It must also include a material.

```
{
    "usual plane" : {
        "type" : "plane",
        "radius" : 500.0,
        "center" :  {
            "x" : 0.0,
            "y" : 0.0,
            "z" : 0.0
        },
        "normal" :  {
            "x" : 0.0,
            "y" : 1.0,
            "z" : 0.0
        },
        "material" : {
            "color" : {
                "r" : 25,
                "g" : 25,
                "b" : 25
            },
            "emmitance" : 0.0,
            "probability diffuse" : 1.0,
            "angle of spread" : 10.0
        } 
}

```

- *obj files* : It support some obj files, it must have a path to the file, a material, a scale parameter and it can optionally include an array of thansformations that will be applied in order.

```
{
    "example bunny" :    {
        "type" : "OBJ",
        "scale" : 7.0,
        "path" : "../assets/bunny.obj",
        "material" : {
            "color" : {
                "r" : 184,
                "g" : 115,
                "b" : 51
            },
            "emmitance" : 0.0,
            "probability diffuse" : 0.3,
            "angle of spread" : 10.0
        },
        "transformations" : [
            { 
                "rx": 0.0,
                "ry": 180.0,
                "rz": 0.0,
                "tx": 0.0,
                "ty": 0.0,
                "tz": 0.0
            }
        ]
    }
}

```
