#include "simulation.h"
/*! \file simulation.cpp */

/*!
Add a pointer to the list of updaters, and give that updater a reference to the
model...
*/
void Simulation::addUpdater(UpdaterPtr _upd, ConfigPtr _config)
    {
    _upd->setModel(_config);
    _upd->setSimulation(getPointer());
    updaters.push_back(_upd);
    };

/*!
Add a pointer to the list of force computers, and give that FC a reference to the
model...
*/
void Simulation::addForce(ForcePtr _force, ConfigPtr _config)
    {
    _force->setModel(_config);
    forceComputers.push_back(_force);
    };


/*!
Set a pointer to the configuration
*/
void Simulation::setConfiguration(ConfigPtr _config)
    {
    configuration = _config;
    Box = _config->Box;
    };

/*!
\post the cell configuration and e.o.m. timestep is set to the input value
*/
void Simulation::setIntegrationTimestep(scalar dt)
    {
    integrationTimestep = dt;
    //auto cellConf = cellConfiguration.lock();
    //cellConf->setDeltaT(dt);
    for (int u = 0; u < updaters.size(); ++u)
        {
        auto upd = updaters[u].lock();
        upd->setDeltaT(dt);
        };
    };

/*!
\post the cell configuration and e.o.m. timestep is set to the input value
*/
void Simulation::setCPUOperation(bool setcpu)
    {
    auto Conf = configuration.lock();
    Conf->setGPU(!setcpu);
    useGPU = !setcpu;

    for (int u = 0; u < updaters.size(); ++u)
        {
        auto upd = updaters[u].lock();
        upd->setGPU(!setcpu);
        };
    for (int f = 0; f < forceComputers.size(); ++f)
        {
        auto frc = forceComputers[f].lock();
        frc->setGPU(!setcpu);
        };
    };

/*!
\pre the updaters already know if the GPU will be used
\post the updaters are set to be reproducible if the boolean is true, otherwise the RNG is initialized
*/
void Simulation::setReproducible(bool reproducible)
    {
    for (int u = 0; u < updaters.size(); ++u)
        {
        auto upd = updaters[u].lock();
        upd->setReproducible(reproducible);
        };
    };

/*!
Calls all force computers, and evaluate the self force calculation if the model demands it
*/
void Simulation::computeForces()
    {
    cout << "computing forces!" << endl;
    auto Conf = configuration.lock();
    if(Conf->selfForceCompute)
        Conf->computeForces(true);
    for (int f = 0; f < forceComputers.size(); ++f)
        {
        auto frc = forceComputers[f].lock();
        bool zeroForces = (f==0 && !Conf->selfForceCompute);
        frc->computeForces(Conf->returnForces(),zeroForces);
        };

    };

/*!
Calls the configuration to displace the degrees of freedom
*/
void Simulation::moveParticles(GPUArray<dVec> &displacements)
    {
    auto Conf = configuration.lock();
    Conf->moveParticles(displacements);
    };

/*!
Call all relevant functions to advance the system one time step; every sortPeriod also call the
spatial sorting routine.
\post The simulation is advanced one time step
*/
void Simulation::performTimestep()
    {
    integerTimestep += 1;
    Time += integrationTimestep;

    //perform any updates, one of which should probably be an EOM
    for (int u = 0; u < updaters.size(); ++u)
        {
        auto upd = updaters[u].lock();
        upd->Update(integerTimestep);
        };
/*
    //spatially sort as necessary
    auto cellConf = cellConfiguration.lock();
    //check if spatial sorting needs to occur
    if (sortPeriod > 0 && integerTimestep % sortPeriod == 0)
        {
        cellConf->spatialSorting();
        for (int u = 0; u < updaters.size(); ++u)
            {
            auto upd = updaters[u].lock();
            upd->spatialSorting();
            };
        };
*/
    };