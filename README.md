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

## How to add a new register-access container

### Creating a new container

A container in fisch abstracts the payload to a register-like access, which can be read, written or both.
To each container, a unique `coordinate_type` coordinate from `halco` describes the range of accessible locations of this register access.
A container provides a recipe to issue a compiletime-fixed number of appropriate UT messages for read and/or write operation as well as decoding of (a compile-time fixed amount of) UT message responses to a read instruction sequence.
This is done through `encode_read`, `encode_write` and `decode` member functions:
```cpp
class MyNewRegister
{
public:
    typedef halco::hicann_dls::vx::MyNewRegisterOnDLS coordinate_type;

    constexpr static size_t encode_read_ut_message_count R;
    constexpr static size_t encode_write_ut_message_count W;
    constexpr static size_t decode_ut_message_count D;

    std::array<hxcomm::vx::ut_message_to_fpga_variant, encode_read_ut_message_count>
    encode_read(coordinate_type const& coord);

    std::array<hxcomm::vx::ut_message_to_fpga_variant, encode_write_ut_message_count>
    encode_write(coordinate_type const& coord);

    void decode(std::array<hxcomm::vx::ut_message_from_fpga_variant, decode_ut_message_count> const& messages);
};
```

Registers with a ranged-number representation use `RantWrapper` from `halco`, see `JTAGClockScaler` in `include/fisch/vx/jtag.h` for an exemplary use.
In addition to the above interface, each register container is to support serialization and an `std::ostream` operator for printing its content, see `JTAGPLLRegister` for an exemplary implementation.

Having done the above steps, the container is to be added in the list of containers in `include/fisch/vx/container.def` in order to generate appropriate `ContainerTicket` and `ContainerVectorTicket` read tickets and instantiate the `read` and `write` functions of the `PlaybackProgramBuilder`.
Add new container headers to `include/fisch/vx/fisch.h` in order to generate python bindings.

### Testing a new container

Software tests of containers are to be written in `C++`. A complete software test includes:
* `General` tests:
  - Testing all member functions for value access
  - Testing the equality and inequality operators
  - Testing value ranges of ranged types if applicable
* Testing the `std::ostream` operator
* Testing serialization coverage with `cereal`, which includes an identity test for altered members and adding the container in the list of tested types in `tests/sw/fisch/vx/test-serialization_cereal.cpp`

See `tests/sw/fisch/vx/test-jtag_clock_scaler.cpp` for an exemplary test.
