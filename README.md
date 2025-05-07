# Intelligent and ingenious STEPS(iSTEPS)

The Ingenious Simulation Toolkit for Electrical Power Systems (iSTEPS) is an open-source power system simulator tailored for balanced large-scale AC-DC hybrid power system analysis. iSTEPS is an independent branch derived from STEPS(https://gitee.com/lichgang/steps/ and https://github.com/changgang/steps), with no intention of merging back into the main repository. 

It provides support for:

- Power flow solutions
- Short circuit analysis
- Eigenvalue analysis
- Electromechanical transient simulation
- Stability optimization control

The data format in iSTEPS is an enhanced version based on the PSS/E raw, seq, and dyr data formats, improving compatibility and usability.

The major system restructuring and improvements from STEPS include:

- Flattening function call hierarchies
- Streamlining code (utilizing templates to aggregate similar code)
- Enhancing simulation computation efficiency
- Gradually integrating AI technologies

Additionally, some API function names will be simplified, adhering to a "five-word maximum" principle for clarity and usability.

- Maintainer: Hengxu Zhang <zhanghx@sdu.edu.cn@sdu.edu.cn> 
- School of Electrical Engineering, Shandong University, 250061 Jinan, Shandong Prov.

# Dependency
STEPS depends on CSparse, CXSparse, and nlohmann/Json. 
The CXSparse is modified to avoid confliction between CSparse and CXSparse by changing prefix cs_ and CS_ to cxs_ and CSX_ .
