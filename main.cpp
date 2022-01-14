#include <iostream>
#include <queue>

double base_timestep = 0.01;
double base_stiffness = 100;
double g_timestep = 0.1;
double mass = 10;
double x0 = 2;

double cutoff(int layer)
{
    return 1.0 / layer;
}

double stiffness(int layer)
{
    return base_stiffness * layer * layer;
}

double timestep(int layer)
{
    return base_timestep / layer;
}

double V(int layer, double x)
{
    double c = cutoff(layer);
    if(x < c)    
        return 0.5 * stiffness(layer) * (c - x) * (c - x);
    else
        return 0;
}

double F(int layer, double x)
{
    double c = cutoff(layer);
    if(x < c)    
        return stiffness(layer) * (c - x);
    else
        return 0;
}

double totEnergy(int nlayers, double x, double xdot)
{
    double result = 0.5 * mass * xdot * xdot;
    result += mass * 9.8 * x;
    for(int i=1; i<=nlayers; i++)
        result += V(i, x);
    return result;
}

int main()
{
    double x = x0;
    double xdot = 0;
    double curtime = 0;
        
    double startEnergy = totEnergy(0, x, xdot);
    double springEnergy = 0;
    int nlayers = 1;
    while(springEnergy < startEnergy)
    {
        springEnergy += V(nlayers, 0);
        nlayers++;
    }
    std::cerr << "Expect to need " << nlayers << " spring layers" << std::endl;
    
    struct Clock
    {
        double nexttime;
        double dt;
        int layer;
        
        bool operator<(const Clock &other ) const
        {
            return nexttime > other.nexttime;
        }
    };
    
    std::priority_queue<Clock> pq;
    pq.push({g_timestep, g_timestep, 0});
    for(int i=1; i<=nlayers; i++)
    {
        pq.push({timestep(i), timestep(i), i});
    }

    while(curtime < 10000.0)
    {
        Clock c = pq.top();
        pq.pop();
        // update position
        double deltat = c.nexttime - curtime;
        x += xdot * deltat;
        // update momentum
        double deltav = 0;
        if(c.layer == 0)
        {
            // gravity
            deltav = -9.8;            
        }
        else
        {
            deltav = F(c.layer, x) / mass;
        }
        xdot += c.dt * deltav;
        curtime = c.nexttime;
        
        if(c.layer == 0)
        {
            // print energy
            std::cerr << "t: " << curtime << " x: " << x << " E: " << totEnergy(nlayers, x, xdot) << std::endl;
        
            // CSV version
            std::cout << curtime << ", " << totEnergy(nlayers, x, xdot) << std::endl;
        }
                
        // push event pack
        c.nexttime += c.dt;
        pq.push(c);
        
    }
}
