#include "duplex.h"

Duplex::Duplex(Settings* c){
    settings = c;
    iterationCap = settings->lookupInt("iterations");
    system = new System(settings);
    
    parameterDimension = settings->lookupInt("parameter.size");
    objectiveDimension = settings->lookupInt("objective.size");
    
    t0 = settings->lookupFloat("initial_temperature");
    temperature = t0;
    initialStepLength = settings->lookupFloat("initial_step_length");
    reinforcementLearningOption = settings->check("reinforcement_learning", "enable");
    shrinkGoalRegionWithTemperateOption = settings->check("shrink_goal_region_with_temperate", "enable");
    if (reinforcementLearningOption){
        minAward = settings->lookupFloat("min_reward");
        maxAward = settings->lookupFloat("max_reward");
        delta = settings->lookupFloat("delta_reward");
    }
    if (settings->check("temperature", "fast")){
        temperatureOption = Temperature::temperaturefast;
    }else if (settings->check("temperature", "boltz")){
        temperatureOption = Temperature::temperatureboltz;
    }else if (settings->check("temperature", "exp")){
        temperatureOption = Temperature::temperatureexp;
    }else{
        cout << "Temperature option not found in the config file. Possible options are [fast, boltz, exp].";
        exit(2);
    }
    
    if (settings->check("annealing", "fast")){
        annealingOption = Annealing::annealingfast;
    }else if (settings->check("annealing", "boltz")){
        annealingOption = Annealing::annealingboltz;
    }else if (settings->check("annealing", "fast-random")){
        annealingOption = Annealing::annealingfastrandom;
    }else if (settings->check("annealing", "boltz-random")){
        annealingOption = Annealing::annealingboltzrandom;
    }else{
        cout << "Annealing option not found in the config file/ Possible options are [fast, boltz]";
        exit(2);
    }
    
    db = new Search(settings, objectiveDimension);
}

Duplex::~Duplex(){
}

void Duplex::insert(int i, State* qnear, State* qnew){
    qnew->setID(i);
    qnew->setParent(qnear);
    db->insert(qnew);
}

void Duplex::optimize(){
    cout << "Executing duplex optimization ..." << endl ;
    auto root = initialize();
    insert(0, root, root);
    int iteration=0;
    State* qnew;
    do{
        auto qprev = globalStep();
        qnew  = localStep(iteration, qprev);
        auto cost = evaluate(qnew);
        cout << cost << endl ;
        insert(iteration, qprev, qnew);
    }while(isConverged(iteration++, qnew));
}

void Duplex::train(){
    optimize();
}

Search* Duplex::getDB(){
    return db;
}

System* Duplex::getSystem(){
    return system;
}

void Duplex::setStat(Stat* s){
    stat = s;
}

Stat* Duplex::getStat(){
    return stat;
}

// Evaluates each states and assign them a score.
// States with lower score are better candidates, state with the minimum score (0) is the optimal solution.
double Duplex::score(State* state, double* maxBound, double* minBound){
    double distance = 0;
    // In functional optimization, we have to evaluat each objective seperately.
    if (settings->check("mode", "fopt")){
        double b = settings->lookupFloat("parameter.b");
        double c0 = settings->lookupFloat("parameter.c0");
        double* boundary = new double[2];
        boundary[0] = b;
        boundary[1] = 0;
        
        double* objectives = state->getObjective();
        double* goals = goal->getObjective();
        int objectiveSize = state->getObjectiveSize();
        //measure objective
        //currently we support the following types of objectives
        //1. boundary: equivalent to the boundary conditions in BVPs. The closer we get to the boundary value, the better
        //2. boundary-strict: similar to boundary, but if we are crossing the bounary value the sameples is not usefull anymore.
        //   for example, for the length of the curve we use bounadry-hard objectives, so the length is strictly less than boundary
        //3. maximize
        //4. minimize
        for (int i = 0; i < objectiveSize; i++){
            auto potentialdistance = state->distance(2, state->getParameter(), boundary);
            auto potentialsum = (state->getParameter()[1] * (b-state->getParameter()[0]) ) / 2.0 ;
            double normalizedDistance = (objectives[i] - goals[i]) / (max[i] - min[i]);
            normalizedDistance *= normalizedDistance;
            if ((objectiveType[i] == "boundary-strict")
                && (objectives[i]>opt[i])                   //if the length of the curve is bigger than the strict boundary, discard this trace
                && (objectives[i]+potentialdistance>opt[i]) //evaluating the potential of this sample: if we even take the direct route to the boundary
                //condition and still the length is more than the bondary, discard this sample
                ){
                normalizedDistance = std::numeric_limits<int>::max();
            }
            if ((objectiveType[i] == "maximize")
                && (objectives[i]>max[i])
                && (objectives[i] + potentialsum > max[i])){
                normalizedDistance = 0;
            }
            distance += normalizedDistance;
        }
        delete[] boundary;    //todo: remove this
    }else{
        //Most of the time, we can use Eucledean distance as the score. Closer to the goal, the better
        distance = goal->distance(state, maxBound, minBound);
    }
    
    state->setScore(distance);
    return distance;
}

void Duplex::save(boost::property_tree::ptree* ptree){
    ptree->add("duplex.version", 1);
    boost::property_tree::ptree& data = ptree->add("duplex.data", "");
    db->save(&data);
    boost::property_tree::ptree& sense = ptree->add("duplex.sensitivity", "");
    stat->sensitivity->save(&sense);
    boost::property_tree::ptree& node = ptree->add("duplex.stat", "");
    
    for (int i = 0; i<db->getSize(); i++){
        boost::property_tree::ptree& iter = node.add("iteration", "");
        iter.add("id", i);
        iter.add("error", stat->getError(i));
        iter.add("distance", stat->getDistance(i));
        iter.put("<xmlattr>.id", i);
    }
}

void Duplex::load(boost::property_tree::ptree* ptree){
    boost::property_tree::ptree& data = ptree->get_child("duplex.data");
    db->load(&data);
    auto pnodes = ptree->get_child("duplex.stat");
    for(auto v: pnodes){
        if(v.first=="iteration"){
            cout << v.second.get<int>("id") << endl;
            stat->error_push_back(v.second.get<double>("error"));
            stat->distance_push_back(v.second.get<double>("distance"));
        }
    }
}
