

#include "../hellopdworld/util.h"

#ifndef TESTING

/**
 * include the interface to Pd 
 */
#include "m_pd.h"

/**
 * define a new "class" 
 */
static t_class *hellopdworld_class;


/**
 * this is the dataspace of our new object
 * we don't need to store anything,
 * however the first (and only) entry in this struct
 * is mandatory and of type "t_object"
 */
typedef struct _hellopdworld {
  t_object  x_obj;
} t_hellopdworld;


/**
 * this method is called whenever a "bang" is sent to the object
 * the name of this function is arbitrary and is registered to Pd in the 
 * hellopdworld_setup() routine
 */
void hellopdworld_bang(t_hellopdworld *x)
{
  /*
   * post() is Pd's version of printf()
   * the string (which can be formatted like with printf()) will be
   * output to wherever Pd thinks it has too (pd's console, the stderr...)
   * it automatically adds a newline at the end of the string
   */
  post(message());
}


/**
 * this is the "constructor" of the class
 * this method is called whenever a new object of this class is created
 * the name of this function is arbitrary and is registered to Pd in the 
 * hellopdworld_setup() routine
 */
void *hellopdworld_new(void)
{
  /*
   * call the "constructor" of the parent-class
   * this will reserve enough memory to hold "t_hellopdworld"
   */
  t_hellopdworld *x = (t_hellopdworld *)pd_new(hellopdworld_class);

  /*
   * return the pointer to the class - this is mandatory
   * if you return "0", then the object-creation will fail
   */
  return (void *)x;
}


/**
 * define the function-space of the class
 * within a single-object external the name of this function is special
 */
void hellopdworld_setup(void) {
  /* create a new class */
  hellopdworld_class = class_new(gensym("hellopdworld"),        /* the object's name is "hellopdworld" */
			       (t_newmethod)hellopdworld_new, /* the object's constructor is "hellopdworld_new()" */
			       0,                           /* no special destructor */
			       sizeof(t_hellopdworld),        /* the size of the data-space */
			       CLASS_DEFAULT,               /* a normal pd object */
			       0);                          /* no creation arguments */

  /* attach functions to messages */
  /* here we bind the "hellopdworld_bang()" function to the class "hellopdworld_class()" -
   * it will be called whenever a bang is received
   */
  class_addbang(hellopdworld_class, hellopdworld_bang); 
}

#endif
