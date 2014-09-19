#include "rjmcmc/util/random.hpp"
#include "rjmcmc/rjmcmc/kernel/raster_variate.hpp"

typedef rjmcmc::mt19937_generator  Engine;  // source of randomness
typedef rjmcmc::raster_variate<2>  Variate; // random 2D point variate in the unit square

int main(int argc, char **argv)
{
    int iter = 10000;
    if(argc>1) iter = atoi(argv[1]);

    double value[] = {

        1, 2, 3, 2, 1,
        0, 3, 3, 3, 0,
        1, 2, 3, 2, 1,
        5, 2, 0, 2, 1,
        1, 2, 3, 2, 1,
        6, 2, 7, 2, 9

    };
    int size[] = {5,6};

    Variate var(value,size);
    Engine& engine = rjmcmc::random();

//    int count[] = {0,0,0, 0,0,0};

    std::cout << "x,y"<< std::endl;
    for(int i=0; i<iter; ++i)
     {
        double val[2];
         double pdf = var(engine,val);
         if(pdf==0) continue; // sampling failed
         if(pdf!=var.pdf(val)) std::cerr << "pdf mismatch : " << pdf << "=" << var.pdf(val) << std::endl;
//         int x = int(val[0]*size[0]);
//         int y = int(val[1]*size[1]);
//         ++count[x+size[0]*y];


         std::cout << val[0]<<","<<val[1] << std::endl;
     }

    return 0;
}
