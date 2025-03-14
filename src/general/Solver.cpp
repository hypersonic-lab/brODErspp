#include "Solver.h"
#include "dfdt.h"
#include "jacobian.h"

typedef boost::numeric::ublas::vector<double> vector_type;

// This implementation of the solver is a little rough... 
// I apologize to anyone that might be hurted by this! :-)

//=============================================================================================================

Solver::Solver(size_t l_n_args, std::string l_file_name)
        : m_n_args(l_n_args),
          s_file_name(l_file_name),
          p_mix(NULL),
          p_setup_props(NULL),
          p_setup_problem(NULL),
          p_problem(NULL), 
          p_sol(NULL),
          p_mesh(NULL),            // specifically for SHOCKING
          p_shock_relations(NULL), // specifically for SHOCKING
          p_data_post(NULL),       // specifically for SHOCKING
          p_data_pre(NULL),        // specifically for SHOCKING
          p_data(NULL),            // specifically for LARSEN
          p_output(NULL)
{ }

//=============================================================================================================

void Solver::initialize(){

    errorInsufficientNumberOfArguments();

    // Parse Input File
    p_setup_props = new SetupProperties(s_file_name);

    //Initializing Mutation++
    setMutationpp();

    // Initializing Abstract Factory Method
    p_setup_problem = SetupProblem::createFactory(p_setup_props->getProblemType(), 
                                                  p_setup_props->getStateModel());

    // =====   Different solvers need different initializations   =====
    s_problem_type = p_setup_props->getProblemType();

    // #####################   S H O C K I N G   #########################
    if(!s_problem_type.compare("shocking") == true) {

      // Data Pre-Post Shock
      p_data_pre = p_setup_problem->getDataPreShock(*p_mix, 
                                             p_setup_props->getInputFileLines());

      p_data_post = p_setup_problem->getDataPostShock(*p_mix);
  
      // Mesh
      p_mesh = new Mesh(p_setup_props->getInputFileLines());
  
      // PostShock
      p_shock_relations = p_setup_problem->getShockRelations(*p_mix);
  
      // Problem
      p_problem = p_setup_problem->getProblem(*p_mix, *p_data_post);
  
      // Setup Output
      p_output = new BasicOutput(*p_mix, *p_data_post, 
                                          p_setup_props->getInputFileLines());
  
      // Apply Shock Relations
      p_shock_relations->applyShockRelations(*p_data_pre, *p_data_post);
    }
    // #####################   L A R S E N   #########################
    else if (!s_problem_type.compare("larsen") == true) {

      // Initialize data object 
      p_data = p_setup_problem->getData(*p_mix, p_setup_props->getInputFileLines());

      // Read file with a flow calculation and also compute enthalpy and mass fluxes
      p_data->readDataFile(); 
      p_data->computeEnthalpy();
      p_data->computeMassDiffusion();

      // Problem
      p_problem = p_setup_problem->getProblem(*p_mix, *p_data);

      // Setup Output
      p_output = new BasicOutput(*p_mix, *p_data, 
                                          p_setup_props->getInputFileLines());
    }
    else { // print error message and exit
      std::cerr << "The problem '" << s_problem_type << "' was choosen.\n";
      std::cerr << "However, the solver only implements 'shocking' and 'larsen' for now!!\n";
      exit(1);
    }

}

// ====================================================================================
// ====================================================================================
// ====================================================================================

void Solver::solve(){
std::cout << "======>  STARTING THE INTEGRATION  <======" << std::endl << std::endl;

  // #######################   S H O C K I N G   ###########################
  if (!s_problem_type.compare("shocking") == true) {

    // Print header
    p_output->printSolutionHeader("shocking");

    // Call solver to solve the problem
    vector_type x = p_data_post->getInitialState();
    //size_t num_of_steps = integrate_const(make_dense_output<rosenbrock4<double> >(1.0e-10, 1.0e-10),
    //                          std::make_pair(dfdt(*p_problem), jacobian(*p_problem)),
    //                          x,
    //                          p_mesh->getXinitial(), p_mesh->getXfinal(), p_mesh->getdX(),
    //                          *p_output);

    size_t num_of_steps = integrate_adaptive(make_dense_output<rosenbrock4<double> >(1.0e-8, 1.0e-8),
                              std::make_pair(dfdt(*p_problem), jacobian(*p_problem)),
                              x,
                              p_mesh->getXinitial(), p_mesh->getXfinal(), p_mesh->getdX(),
                              *p_output);

    //typedef runge_kutta_cash_karp54< vector_type > rkck54;
    //typedef controlled_runge_kutta< rkck54 > ctrl_rkck54;
    //integrate_adaptive( ctrl_rkck54(), dfdt(*p_problem), x , p_mesh->getXinitial() , p_mesh->getXfinal() , p_mesh->getdX());

  }
  // #######################    L A R S E N    ############################# 
  else if(!s_problem_type.compare("larsen") == true) {

    // Print header
    p_output->printSolutionHeader("larsen");

    // Get initial state from external field
    vector_type x = p_data->getInitialState();

    // Now solve step by step
    for(size_t ii = 0; ii < p_data->getStrNele()-1; ++ii)
    {
      std::cout << " LARSEN - Step " << ii << " of " 
                << p_data->getStrNele()-1 << std::endl;

      // Set current step into Data object and get position value
      p_data->setCurrentStep(ii);
      
      double xNow[2];
      double dummy[2];
      p_data->getStepValues(xNow, dummy, dummy);

      // Integrate up to next point on streamline
      size_t num_of_steps = 
          integrate_adaptive(make_dense_output< rosenbrock4< double > >(1.0e-5,1.0e-5), 
                             std::make_pair(dfdt(*p_problem), jacobian(*p_problem)),
                             x, 
                             xNow[0], xNow[1], 
                             0.000001,   // for now the step is hardcoded!
                             *p_output);
    }

  }
}

//=============================================================================================================

Solver::~Solver(){
    if (p_mix != NULL) delete p_mix;
    if (p_setup_props != NULL) delete p_setup_props;
    if (p_setup_problem != NULL) delete p_setup_problem;
    if (p_problem != NULL) delete p_problem;
    if (p_shock_relations != NULL) delete p_shock_relations;
    if (p_mesh != NULL) delete p_mesh;
    if (p_data_pre != NULL) delete p_data_pre;
    if (p_data_post != NULL) delete p_data_post;
    if (p_data != NULL) delete p_data_post;
    if (p_output != NULL) delete p_output;
}

//=============================================================================================================

inline void Solver::setMutationpp(){
    Mutation::MixtureOptions opts(p_setup_props->getMixture());
    opts.setStateModel(p_setup_props->getStateModel());
    opts.setThermodynamicDatabase(p_setup_props->getThermoDB());
    p_mix = new Mutation::Mixture(opts);
}

//=============================================================================================================

inline void Solver::errorInsufficientNumberOfArguments(){
    if ( m_n_args < 2 ) {
        std::cout << "Error: The name of the input file for the solver should be provided!" << std::endl;
        exit(1);
    }
}

