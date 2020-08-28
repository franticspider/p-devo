//
//  euclid.c
//  euclid
//
//  Created by hdez on 7/8/16.
//  based on https://github.com/cheetomoskeeto/pd-externals-c
//

//Get the Pure Data headers from: https://github.com/pure-data/pure-data/blob/master/src/m_pd.h
//#include <stdlib.h>
//#include <string.h>
#include "m_pd.h"




////////////////////////////////////////////////////////////////////////////////
// JOHN TUFFEN'S WONKYSTUFF RNG CODE: 
// See http://doitwireless.com/2014/06/26/8-bit-pseudo-random-number-generator/
// https://en.wikipedia.org/wiki/Linear-feedback_shift_register#Galois_LFSRs
////////////////////////////////////////////////////////////////////////////////
uint8_t rnd(void)
{
  static uint8_t r = 0x23;
  uint8_t lsb = r & 1;
  r >>= 1;
  r ^= (-lsb) & 0xB8;
  return r;
}



////////////////////////////////////////////////////////////////////////////////
// Euclid functions
////////////////////////////////////////////////////////////////////////////////
//https://www.computermusicdesign.com/simplest-euclidean-rhythm-algorithm-explained/

#define MAX_STEPS (64)

/* R version: 

						euclid <- function(steps,pulses,rotation){
							storedRhythm <- vector(length=steps)
							bucket <- 0
							for(ii in 1:steps){
								bucket <- bucket + pulses
								if(bucket >= steps){
									bucket <- bucket - steps
									storedRhythm[ii]<-1
								}
								else{
									storedRhythm[ii]<-0
								}
								message(sprintf("%d",storedRhythm[ii]),appendLF = F)
							}
							message("")
							return(storedRhythm)
						}
							
							
						euclid(13,5,1)
						euclid(13,10,1)
						euclid(61,23,1)
*/


//TODO: do it in-place
//function rotateSeq(seq, rotate){
//    var output = new Array(seq.length); //new array to store shifted rhythm
//    var val = seq.length - rotate;
//
//    for( var i=0; i < seq.length ; i++){
//        output[i] = seq[ Math.abs( (i+val) % seq.length) ];
//    }
//
//    return output;
//}

//Rotate in-place from https://www.codewhoop.com/array/rotation-in-place.html
int gcd(int a, int b)
{
  if(b==0)
    return a;
  else
    return gcd(b, a%b);
}

void ArrayRotate (char A[], int n, int k)
{
  int d=-1,i,temp,j;
  for(i=0;i<gcd(n,k);i++){
    j=i;
    temp=A[i];
    while(1){
      d=(j+k)%n;
      if(d==i)
        break;
      A[j]=A[d];
      j=d;
    }
    A[j]=temp;
  }
}



void euclid(int steps, int pulses, int rotation, char storedRhythm[]){
	int bucket = 0;
  for(int ii=0;ii<steps;ii++){
		bucket += pulses;
    if(bucket >= steps){
			bucket -= steps;
			storedRhythm[ii] = '1';
		}else{
			storedRhythm[ii] = '0';
		}
	}
 
  if(rotation)
		ArrayRotate(storedRhythm, steps, rotation);

}






////////////////////////////////////////////////////////////////////////////////
// PD interface
////////////////////////////////////////////////////////////////////////////////

//Create a static pointer to the class; it will be initialized in the _setup() method
static t_class *euclid_class;

//Carve out memory for the class data model and typdef it
typedef struct _euclid {
    t_object    x_obj;
    t_int       init_count, current_count;
    t_int       mod_A, mod_B;
    t_inlet     *in_mod_A, *in_mod_B, *in_rot;
    t_outlet    *out_A, *out_B, *out_synch, *out_count;
    char 				storedRhythm[MAX_STEPS];
} t_euclid;

//Set the ratio (A:B)
void euclid_setMods(t_euclid *x, t_floatarg f1, t_floatarg f2){
    //Ensure no negative numbers or 0s are received
    //A
    x->mod_A = (f1 <= 0) ? 1 : f1;
    //B
    x->mod_B = (f2 <= 0) ? 1 : f2;
}

//Reset the count to start at 0
void euclid_resetCount(t_euclid *x){
    //Initialize the counts to start at 0 again
    x->init_count = 0;
    x->current_count = x->init_count;
}

//Perform on bang
void euclid_onBangMsg(t_euclid *x){
    //Create local vars for convenience
    t_int mod_A  = x->mod_A;
    t_int mod_B   = x->mod_B;
    t_int mod_synch = mod_A * mod_B;
    t_int n       = x->current_count;
    
    //Bangs
    //post("beat %d: pulse = %c",n,x->storedRhythm[n]);
    if(x->storedRhythm[n]=='1'){
        outlet_bang(x->out_A);
		}




    /*if(n % mod_synch == 0){
        //Bangs will be synched when the current count % (A*B) == 0
        //On synch, bang all outputs and reset the current count
        outlet_bang(x->out_A);
        outlet_bang(x->out_B);
        outlet_bang(x->out_synch);
        
        //Reset the count
        x->current_count = 0;
    } else {
        // the current count % A == 0 will cause a bang to out_B
        if(n % mod_A == 0) outlet_bang(x->out_A);
        
        //Same as above but with B
        if(n % mod_B == 0) outlet_bang(x->out_B);
    }*/
    
    //Always output the current count
    outlet_float(x->out_count, x->current_count);
    
    //Increment the current count
    x->current_count++;
    if(x->current_count >= x->mod_A)
      x->current_count =0;


}

void euclid_onResetMsg(t_euclid *x){
    euclid_resetCount(x);
}

//Handle lists: [A B(
void euclid_onListMsg(t_euclid *x, t_symbol *s, t_int argc, t_atom *argv){
    switch(argc){
            //Lists must have two arguments (to set both A and B); otherwise we'll throw an error
        case 2:
            //Set mod_A and mod_B
            euclid_setMods(x, atom_getfloat(argv), atom_getfloat(argv+1));
            
            //Reset the counts
            euclid_resetCount(x);
            break;
        default:
            error("[euclid ]: two arguments are needed to set a new ratio");
    }
}

//Set A part of ratio; however, don't reset
void euclid_onSet_A(t_euclid *x, t_floatarg f){
    //New number for A of A:B
    euclid_setMods(x, f, x->mod_B);
}

//Set B part of ratio; however, don't reset
void euclid_onSetB(t_euclid *x, t_floatarg f){
    //New number for B of A:B
    euclid_setMods(x, x->mod_A, f);
}

//The _new method is a class constructor
//It is required to be named [filename]_new()
void *euclid_new(t_floatarg f1, t_floatarg f2){
    //Create an instance of the euclid class
    t_euclid *x = (t_euclid *)pd_new(euclid_class);
    
    //Initialize the counts
    //Since we'll be doing this in a few different locations, we'll create a method to accomplish this
    euclid_resetCount(x);
    
    //Initialize the mods (e.g. num % mod_A)
    //Since we'll be doing this in a few different locations, we'll create a method to accomplish this
    euclid_setMods(x, f1, f2);

    //Create inlets
    //The left-most inlet is created and destroyed automatically
    //Any additional inlets needed must be explicilty created
    //Inlets are created from left to right on the object
    //Handles (pointers, really) are stored so that we can free them later
    x->in_mod_A = inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_float, gensym("ratio_A"));
    x->in_mod_B = inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_float, gensym("ratio_B"));
    x->in_rot = inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_float, gensym("rotation"));

    //Create outlets
    //Unlike the first inlet, the first isn' created automatically
    //Any outlets needed must be explicitly created
    //Outlets are created from left to right on the object
    //Handles (pointers, really) are stored so that we can send data to the outlets and/or free them later
    x->out_A = outlet_new(&x->x_obj, &s_bang);
    x->out_B = outlet_new(&x->x_obj, &s_bang);
    x->out_synch = outlet_new(&x->x_obj, &s_bang);
    x->out_count = outlet_new(&x->x_obj, &s_float);


    //Euclid stuff:
    //memset(x->storedRhythm,0,MAX_STEPS*sizeof(char));
    for(int ii=0;ii<MAX_STEPS;ii++)x->storedRhythm[ii]=0;
    euclid(x->mod_A,x->mod_B,0,x->storedRhythm);
    post("pattern is %s",x->storedRhythm);
      
    //Return the instance
    return (void *)x;
}

void euclid_free(t_euclid *x){
    //If we create any inlets or outlets ourselves, we need to free them to avoid memory leaks
    inlet_free(x->in_mod_A);
    inlet_free(x->in_mod_B);
    outlet_free(x->out_A);
    outlet_free(x->out_B);
    outlet_free(x->out_synch);
    outlet_free(x->out_count);

    //free(x->storedRhythm);
}

//The _setup method initializes the class in memory
//This method is the place where class behaviors are added
//It is required to be named [filename]_setup()
void euclid_setup(void){
    //Initialize the class
    euclid_class = class_new(gensym("euclid"),
                               (t_newmethod)euclid_new,
                               (t_method)euclid_free,
                               sizeof(t_euclid),
                               CLASS_DEFAULT,
                               A_DEFFLOAT, //A of A:B
                               A_DEFFLOAT, //B of A:B
                               0);
    //Define the handling of various inputs
    
    //Bang
    class_addbang(euclid_class, (t_method)euclid_onBangMsg);

    //List: set the ratio and reset
    class_addlist(euclid_class, (t_method)euclid_onListMsg);
    
    //Reset: start counting at 0 again
    class_addmethod(euclid_class,
                    (t_method)euclid_onResetMsg,
                    gensym("reset"),
                    0);

    //ratio_A: set A but don't reset
    class_addmethod(euclid_class,
                    (t_method)euclid_onSet_A,
                    gensym("ratio_A"),
                    A_DEFFLOAT, //A of A:B
                    0);
    
    //ratio_B: set B but don't reset
    class_addmethod(euclid_class,
                    (t_method)euclid_onSetB,
                    gensym("ratio_B"),
                    A_DEFFLOAT, //B of A:B
                    0);
    
    //Set the help file
    //This will look for "euclid-help.pd"
    class_sethelpsymbol(euclid_class, gensym("euclid"));


}
