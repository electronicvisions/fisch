# FPGA Instruction Set Compiler for HICANN

## How to build

```bash
# on hel, cat /etc/motd for latest instructions

module load waf

mkdir my_project && cd my_project/
module load localdir

# first build
waf setup --project fisch
srun -p compile singularity exec --app visionary-dls /containers/stable/latest waf configure install --test-execnone

# run (software) tests
singularity exec --app visionary-dls /containers/stable/latest waf install --test-execall
```

## Usage

Typically you would use the shared object in another project.
See `tests/hw/fisch/vx/test-readout_jtag_id.cpp` for an example.


## Contributing

In case you encounter bugs, please [file a work package](https://brainscales-r.kip.uni-heidelberg.de/projects/fisch/work_packages/) describing all steps required to reproduce the problem.
Don't just post in the chat, as the probability to get lost is very high.

Before committing any changes, make sure to run `git-clang-format` and add the resulting changes to your commit.
