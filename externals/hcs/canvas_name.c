#include <stdio.h>
#include <string.h>
#include <m_pd.h>
#include "g_canvas.h"

#define DEBUG(x)

static t_class *canvas_name_class;
static t_canvas *canvas;

typedef struct _canvas_name
{
    t_object x_obj;
    t_atom x_atom;
    t_symbol *x_canvas_name;
    t_symbol *x_remote_name;
} t_canvas_name;

static void canvas_name_bang(t_canvas_name *x)
{
    if (x->x_atom.a_type == A_SYMBOL)
    {
        canvas=(t_canvas *)pd_findbyclass(x->x_remote_name, canvas_class);
        char buf[MAXPDSTRING];

        snprintf(buf, MAXPDSTRING, ".x%lx.c", (long unsigned int)canvas);
        x->x_canvas_name = gensym(buf);
    }
    outlet_symbol(x->x_obj.ob_outlet,x->x_canvas_name);
}

static void *canvas_name_new(t_symbol *s, int argc, t_atom *argv)
{
    t_atom a;
    if (argc == 0)
    {
        argc = 1;
        SETFLOAT(&a, 0);
        argv = &a;
    }
    t_canvas_name *x = (t_canvas_name *)pd_new(canvas_name_class);
    x->x_atom = *argv;
    if (argv->a_type == A_FLOAT)
    { // thx to IOhannes's iemguts:
        t_glist *glist=(t_glist *)canvas_getcurrent(); 
        canvas=(t_canvas *)glist_getcanvas(glist);
        int depth=(int)atom_getint(&x->x_atom);

        if(depth<0)depth=0;
        while(depth && canvas) {
          canvas=canvas->gl_owner;
          depth--;
        }
        char buf[MAXPDSTRING];

        snprintf(buf, MAXPDSTRING, ".x%lx.c", (long unsigned int)canvas);
        x->x_canvas_name = gensym(buf);
    }
    else
    {
        x->x_remote_name = (t_symbol *)atom_getsymbol(&x->x_atom);
    }
    
    outlet_new(&x->x_obj, &s_symbol);

    return(x);
}

void canvas_name_setup(void)
{
    canvas_name_class = class_new(gensym("canvas_name"),
        (t_newmethod)canvas_name_new, NULL,
        sizeof(t_canvas_name), 0, A_GIMME, 0);

    class_addbang(canvas_name_class, (t_method)canvas_name_bang);
}
