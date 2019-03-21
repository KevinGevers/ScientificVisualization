#include <stdio.h>              //for printing the help text
#include <stdlib.h>
#include <math.h>               //for various math functions
#include <limits>
#include "simulation.h"

Simulation::Simulation(int n)
{
    dim = n;
    init_simulation(n);
}

//float Simulation::get_length(float x, float y) {return sqrt(x*x + y*y);}

//init_simulation: Initialize simulation data structures as a function of the grid size 'n'.
//                 Although the simulation takes place on a 2D grid, we allocate all data structures as 1D arrays,
//                 for compatibility with the FFTW numerical library.
void Simulation::init_simulation(int n)
{
    size_t dim;

    dim     = static_cast<size_t>(n * 2*(n/2+1)) * sizeof(double);        //Allocate data structures
    vx      = static_cast<double*>(malloc(dim));
    vy      = static_cast<double*>(malloc(dim));
    vx0     = static_cast<double*>(malloc(dim));
    vy0     = static_cast<double*>(malloc(dim));
    dim     = static_cast<size_t>(n * n) * sizeof(double);
    fx      = static_cast<double*>(malloc(dim));
    fy      = static_cast<double*>(malloc(dim));
    rho     = static_cast<double*>(malloc(dim));
    rho0    = static_cast<double*>(malloc(dim));
//    plan_rc = rfftw2d_create_plan(n, n, double_TO_COMPLEX, FFTW_IN_PLACE);
//    plan_cr = rfftw2d_create_plan(n, n, FFTW_COMPLEX_TO_REAL, FFTW_IN_PLACE);

    for (int i = 0; i < n * n; i++)                      //Initialize data structures to 0
    { vx[i] = vy[i] = vx0[i] = vy0[i] = fx[i] = fy[i] = rho[i] = rho0[i] = static_cast<double>(0.0f); }
}


//FFT: Execute the Fast Fourier Transform on the dataset 'vx'.
//     'dirfection' indicates if we do the direct (1) or inverse (-1) Fourier Transform
void Simulation::FFT(int direction,void* vx)
{
    fftw_plan plan;

    if (direction == 1) {
        plan = fftw_plan_dft_r2c_2d(dim, dim, static_cast<double*>(vx), static_cast<fftw_complex*>(vx), FFTW_ESTIMATE);
    } else {
        plan = fftw_plan_dft_c2r_2d(dim, dim, static_cast<fftw_complex*>(vx), static_cast<double*>(vx), FFTW_ESTIMATE);
    }

    fftw_execute(plan);
}

int Simulation::clamp(float x)
{ return ((x)>=0.0f?(static_cast<int>(x)):(-(static_cast<int>(1-(x))))); }

//solve: Solve (compute) one step of the fluid flow simulation
void Simulation::solve(int n, double* vx, double* vy, double* vx0, double* vy0, double visc, double dt)
{
    double x, y, x0, y0, f, r, U[2], V[2], s, t;
    int i, j, i0, j0, i1, j1;

    for (i=0;i<n*n;i++)
    { vx[i] += dt*vx0[i]; vx0[i] = vx[i]; vy[i] += dt*vy0[i]; vy0[i] = vy[i]; }

    for ( x=0.5f/n,i=0 ; i<n ; i++,x+=1.0f/n )
       for ( y=0.5f/n,j=0 ; j<n ; j++,y+=1.0f/n )
       {
          x0 = n*(x-dt*vx0[i+n*j])-0.5f;
          y0 = n*(y-dt*vy0[i+n*j])-0.5f;
          i0 = clamp(x0); s = x0-i0;
          i0 = (n+(i0%n))%n;
          i1 = (i0+1)%n;
          j0 = clamp(y0); t = y0-j0;
          j0 = (n+(j0%n))%n;
          j1 = (j0+1)%n;
          vx[i+n*j] = (1-s)*((1-t)*vx0[i0+n*j0]+t*vx0[i0+n*j1])+s*((1-t)*vx0[i1+n*j0]+t*vx0[i1+n*j1]);
          vy[i+n*j] = (1-s)*((1-t)*vy0[i0+n*j0]+t*vy0[i0+n*j1])+s*((1-t)*vy0[i1+n*j0]+t*vy0[i1+n*j1]);
       }

    for(i=0; i<n; i++)
      for(j=0; j<n; j++)
      {  vx0[i+(n+2)*j] = vx[i+n*j]; vy0[i+(n+2)*j] = vy[i+n*j]; }

    FFT(1,vx0);
    FFT(1,vy0);

    for (i=0;i<=n;i+=2)
    {
       x = 0.5f*i;
       for (j=0;j<n;j++)
       {
          y = j<=n/2 ? static_cast<double>(j) : static_cast<double>(j)-n;
          r = x*x+y*y;
          if ( r==0.0f ) continue;
          f = static_cast<double>(exp(-r*dt*visc));
          U[0] = vx0[i  +(n+2)*j]; V[0] = vy0[i  +(n+2)*j];
          U[1] = vx0[i+1+(n+2)*j]; V[1] = vy0[i+1+(n+2)*j];

          vx0[i  +(n+2)*j] = f*((1-x*x/r)*U[0]     -x*y/r *V[0]);
          vx0[i+1+(n+2)*j] = f*((1-x*x/r)*U[1]     -x*y/r *V[1]);
          vy0[i+  (n+2)*j] = f*(  -y*x/r *U[0] + (1-y*y/r)*V[0]);
          vy0[i+1+(n+2)*j] = f*(  -y*x/r *U[1] + (1-y*y/r)*V[1]);
       }
    }

    FFT(-1,vx0);
    FFT(-1,vy0);

    f = 1.0/(n*n);
    for (i=0;i<n;i++)
       for (j=0;j<n;j++)
       { vx[i+n*j] = f*vx0[i+(n+2)*j]; vy[i+n*j] = f*vy0[i+(n+2)*j]; }
}


// diffuse_matter: This function diffuses matter that has been placed in the velocity field. It's almost identical to the
// velocity diffusion step in the function above. The input matter densities are in rho0 and the result is written into rho.
void Simulation::diffuse_matter(int n, double *vx, double *vy, double *rho, double *rho0, double dt)
{
    double x, y, x0, y0, s, t;
    int i, j, i0, j0, i1, j1;

    rho_min = std::numeric_limits< double >::max();
    rho_max = 0;

    for ( x=0.5f/n,i=0 ; i<n ; i++,x+=1.0f/n )
        for ( y=0.5f/n,j=0 ; j<n ; j++,y+=1.0f/n )
        {
            x0 = n*(x-dt*vx[i+n*j])-0.5f;
            y0 = n*(y-dt*vy[i+n*j])-0.5f;
            i0 = clamp(x0);
            s = x0-i0;
            i0 = (n+(i0%n))%n;
            i1 = (i0+1)%n;
            j0 = clamp(y0);
            t = y0-j0;
            j0 = (n+(j0%n))%n;
            j1 = (j0+1)%n;

            rho[i+n*j] = (1-s)*((1-t)*rho0[i0+n*j0]+t*rho0[i0+n*j1])+s*((1-t)*rho0[i1+n*j0]+t*rho0[i1+n*j1]);

            if (rho[i+n*j] > rho_max)
                rho_max = rho[i+n*j];
            if (rho[i+n*j] < rho_min)
                rho_min = rho[i+n*j];
        }
}

//set_forces: copy user-controlled forces to the force vectors that are sent to the solver.
//            Also dampen forces and matter density to get a stable simulation.
void Simulation::set_forces(void)
{
    int i;
    for (i = 0; i < dim * dim; i++)
    {
        rho0[i]  = 0.995 * rho[i];
        fx[i] *= 0.85;
        fy[i] *= 0.85;
        vx0[i]    = fx[i];
        vy0[i]    = fy[i];
    }
}
//// emilio's code, remove before submitting!
//void Simulation::calc_divergence(int vector_data_set, int DIM){

//        fftw_real* datax;
//        fftw_real* datay;
//        switch(vector_data_set){
//        case 0:
//            datax = this->get_vx();
//            datay = this->get_vy();
//        break;
//        case 1:
//            datax = this->get_fx();
//            datay = this->get_fy();
//        break;
//        }


//        float  wn = 2.0 / DIM;   // Grid cell width
//        float  hn = 2.0 / DIM;  // Grid cell height

//        int i;
//        for (i = 0; i < DIM * DIM; i++)
//        {
//            float upper = this->get_length(datax[i-DIM], datay[i-DIM]);
//            float below = this->get_length(datax[i+DIM], datay[i+DIM]);
//            float left =  this->get_length(datax[i-1], datay[i-1]);
//            float right = this->get_length(datax[i+1], datay[i+1]);

//            float diff_x = ((left-right)/wn);
//            float diff_y = ((upper-below)/hn);

//            float result_d = (diff_x + diff_y);
//            if (result_d > max_divergence)
//                max_divergence = result_d;
//            if (result_d < min_divergence)
//                min_divergence = result_d;

//            divergence[i] =result_d;
//       }
//}



void Simulation::calc_divergence(int vectorField)
{
    double *dataX, *dataY;
    if (vectorField)
    {
        dataX = this->get_fx();
        dataY = this->get_fy();
    } else {
        dataX = this->get_vx();
        dataY = this->get_vy();
    }

}

//do_one_simulation_step: Do one complete cycle of the simulation:
//      - set_forces:
//      - solve:            read forces from the user
//      - diffuse_matter:   compute a new set of velocities
void Simulation::do_one_simulation_step(void)
{
    set_forces();
    solve(dim, vx, vy, vx0, vy0, static_cast<double>(visc), dt);
    diffuse_matter(dim, vx, vy, rho, rho0, dt);
}

void Simulation::drag(int mx, int my, int winWidth, int winHeight)
{
    int xi,yi,X,Y; double  dx, dy, len;
    static int lmx=0,lmy=0;				//remembers last mouse location

    // Compute the array index that corresponds to the cursor location
    xi = (int)clamp((double)(dim + 1) * ((double)mx / (double)winWidth));
    yi = (int)clamp((double)(dim + 1) * ((double)(winHeight - my) / (double)winHeight));

    X = xi; Y = yi;

    if (X > (dim - 1))  X = dim - 1; if (Y > (dim - 1))  Y = dim - 1;
    if (X < 0) X = 0; if (Y < 0) Y = 0;

    // Add force at the cursor location
    my = winHeight - my;
    dx = mx - lmx; dy = my - lmy;
    len = sqrt(dx * dx + dy * dy);
    if (len != 0.0) {  dx *= 0.1 / len; dy *= 0.1 / len; }
    fx[Y * dim + X] += dx;
    fy[Y * dim + X] += dy;
    rho[Y * dim + X] = 10.0;
    lmx = mx; lmy = my;
}

int Simulation::get_dim()
{
    return dim;
}


double Simulation::get_rho(int idx)
{
    return rho[idx];
}

double Simulation::get_rho_max()
{
    return rho_max;
}

double Simulation::get_rho_min()
{
    return rho_min;
}

float Simulation::get_rho_maxf()
{
    return static_cast<float>(rho_max);
}

float Simulation::get_rho_minf()
{
    return static_cast<float>(rho_min);
}


float Simulation::get_rhof(int idx)
{
    return static_cast<float>(rho[idx]);
}

double Simulation::get_fx(int idx)
{
    return fx[idx];
}

double* Simulation::get_fx()
{
    return fx;
}

double Simulation::get_fy(int idx)
{
    return fy[idx];
}

double* Simulation::get_fy()
{
    return fy;
}



double Simulation::get_vx(int idx)
{
    return vx[idx];
}

double* Simulation::get_vx()
{
    return vx;
}

double Simulation::get_vy(int idx)
{
    return vy[idx];
}

double* Simulation::get_vy()
{
    return vy;
}

float Simulation::get_vxf(int idx)
{
    return static_cast<float>(vx[idx]);
}

float Simulation::get_vyf(int idx)
{
    return static_cast<float>(vy[idx]);
}

float Simulation::get_fxf(int idx)
{
    return static_cast<float>(fx[idx]);
}

float Simulation::get_fyf(int idx)
{
    return static_cast<float>(fy[idx]);
}

void Simulation::set_viscosity(float viscosity)
{
    visc = viscosity;
}

void Simulation::set_dt(double new_dt)
{
    dt = new_dt;
}
