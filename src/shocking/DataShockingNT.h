#ifndef DATASHOCKINGNT_H
#define DATASHOCKINGNT_H

#include <string>
#include "mutation++.h"

#include "Data.h"

class DataShockingNT: public Data {
public:
    DataShockingNT(Mutation::Mixture& l_mix);
    DataShockingNT(Mutation::Mixture& l_mix, 
                       const std::vector< std::string > l_input_file);
    ~DataShockingNT();
    
    vector_type getInitialState(){ return v_X; }
    double mdot() const { return m_mdot; }
    int nEquations() const { return n_eq; }
    
    std::vector<double> getPartialDensities() const { return v_rhoi; }
    double getPressure() const { return m_P; }
    double getVelocity() const { return m_V; }
    double getTTrans() const { return v_T[0]; }
    std::vector<double> getTemperatures() const { return v_T; }
    std::vector<double> getMassFractions() const { return v_yi; }
    
    void setPressure(const double& l_P){ m_P = l_P; }
    void setVelocity(const double& l_V){ m_V = l_V; }
    void setTTrans(const double& l_T){ v_T[0] = l_T;}
    void setTemperatures(const std::vector<double>& l_T){ v_T = l_T; }
    void setMassFractions(const std::vector<double>& l_yi){ v_yi = l_yi; }
    
    void buildState(); // Helper function which finalizes the state.
    void buildStatePostShock(); // Helper function which finalizes the state after the shock.

private:
    // Variables
    Mutation::Mixture& m_mix;
    const size_t n_sp;
    const size_t n_meq;
    const size_t n_eneq;
    const size_t n_eq;
    const size_t pos_V;
    const size_t pos_T;

    double m_P;
    double m_V;
    double m_rho;
    double m_mdot;
    std::vector<double> v_rhoi;
    std::vector<double> v_xi;
    std::vector<double> v_yi;
    std::vector<double> v_T;

    vector_type v_X;

    // Functions
    void inputFileParse(const std::vector< std::string > l_input_file);

    void checkAndPrintFreeStream();
    inline void fillStateVectorX();
};

#endif /* DATASHOCKINGNT_H */
