#ifndef SIMULATION_H
#define SIMULATION_H

#include <fftw3.h>              //the numerical simulation FFTW library

class Simulation
{
public:
    Simulation(int n);
    void init_simulation(int n);
    void FFT(int direction,void* vx);
    int clamp(float x);
    float max(float x, float y);
    void solve(int n, double* vx, double* vy, double* vx0, double* vy0, double visc, double dt);
    void diffuse_matter(int n, double *vx, double *vy, double *rho, double *rho0, double dt);
    void set_forces(void);
    void do_one_simulation_step(void);
    void drag(int mx, int my, int winWidth, int winHeight);

    int get_dim();
    double get_rho(int idx);
    double get_rho0(int idx);

    double get_vx(int idx);
    double get_vy(int idx);
    double get_vx0(int idx);
    double get_vy0(int idx);


    //--- SIMULATION PARAMETERS ------------------------------------------------------------------------
    double dt = 0.4;				//simulation time step
    float visc = 0.001f;				//fluid viscosity
    double *vx, *vy;             //(vx,vy)   = velocity field at the current moment
    double *vx0, *vy0;           //(vx0,vy0) = velocity field at the previous moment
    double *fx, *fy;	            //(fx,fy)   = user-controlled simulation forces, steered with the mouse
    double *rho, *rho0;			//smoke density at the current (rho) and previous (rho0) moment
//    rfftwnd_plan plan_rc, plan_cr;  //simulation domain discretization

private:
    int dim;

};

#endif // SIMULATION_H
