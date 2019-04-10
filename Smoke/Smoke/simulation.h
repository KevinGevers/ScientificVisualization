#ifndef SIMULATION_H
#define SIMULATION_H

#include <fftw3.h>              //the numerical simulation FFTW library

class Simulation
{
public:
    Simulation(int n);
    void init_simulation();
    void FFT(int direction,void* vx);
    int clamp(float x);
    float max(float x, float y);
    void solve(int n, double* vx, double* vy, double* vx0, double* vy0, double visc, double dt);
    void diffuse_matter(int n, double *vx, double *vy, double *rho, double *rho0, double dt);
    void set_forces(void);
    void calc_divergence(int mode);

    void do_one_simulation_step(void);
    void drag(int mx, int my, int winWidth, int winHeight);

    int get_dim();

    double get_rho(int idx);
    float get_rhof(int idx);

    double get_vx(int idx);
    double *get_vx();
    double get_vy(int idx);
    double *get_vy();
    float get_vxf(int idx);
    float get_vyf(int idx);

    void set_viscosity(float viscosity);
    void set_dt(double new_dt);

    double get_rho_max();
    double get_rho_min();

    float get_rho_maxf();
    float get_rho_minf();

    double get_fx(int idx);
    double *get_fx();
    double get_fy(int idx);
    double *get_fy();
    float get_fxf(int idx);
    float get_fyf(int idx);
    float get_divergence(int idx);
    float get_divergence_max();
    float get_divergence_min();

private:
    int dim;
    float divergence_max = 0;
    float divergence_min = 0;

    //--- SIMULATION PARAMETERS ------------------------------------------------------------------------
    double dt = 0.4;				//simulation time step
    float visc = 0.001f;				//fluid viscosity
    double *vx, *vy;             //(vx,vy)   = velocity field at the current moment
    double *vx0, *vy0;           //(vx0,vy0) = velocity field at the previous moment
    double *fx, *fy;	            //(fx,fy)   = user-controlled simulation forces, steered with the mouse
    double *rho, *rho0;			//smoke density at the current (rho) and previous (rho0) moment
    double rho_max;
    double rho_min;
    double *divergence;
};

#endif // SIMULATION_H
