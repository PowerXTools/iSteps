# Intelligent Simulation Toolkit for Electrical Power Systems (iSTEPS)

The Intelligent Simulation Toolkit for Electrical Power Systems (iSTEPS) is an open-source power system simulator tailored for balanced large-scale AC-DC hybrid power system analysis. iSTEPS is an independent branch derived from STEPS(https://gitee.com/lichgang/steps/ and https://github.com/changgang/steps), with no intention of merging back into the main repository. 

It aims to make major system restructuring and improvements from STEPS, include:

- Flattening function call hierarchies
- Streamlining code (utilizing templates to aggregate similar code)
- Enhancing simulation computation efficiency, smart and high efficiency
- Gradually integrating AI technologies

It provides support for:

- Power flow solutions
- Short circuit analysis
- Eigenvalue analysis
- Electromechanical transient simulation
- Stability quantification assessment and optimal control

The data format in iSTEPS is an enhanced version based on the PSS/E raw, seq, and dyr data formats, improving compatibility and usability.


Additionally, some API function names will be simplified, adhering to a "five-word maximum" principle for clarity and usability.

- Maintainer: Hengxu Zhang <zhanghx@sdu.edu.cn> 
- School of Electrical Engineering, Shandong University, 250061 Jinan, Shandong Prov.

# Programming Languages and Development Environment

To ensure cross-platform compatibility, the core computational program is developed in C++ (compatible with the GNU C++20 standard), while extensions for external calls are written in Python.

It is compatible with the Code::Blocks development environment and includes CMake management files, making it suitable for various development environments such as VS Code, Trae, and others.

# Dependency
STEPS depends on CSparse, CXSparse, and nlohmann/Json. 