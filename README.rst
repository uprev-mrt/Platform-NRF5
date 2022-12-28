Platform-NRF5 
=============


To use the NRF5 Abstraction layer, create a repo with an NRF5 project.


Use the `mrt-config <https://github.com/uprev-mrt/mrtutils>`_ tool to add in submodules. Make sure to import the **Platforms/Common** and **Platforms/NRF5 modules**
 

Setting the Platform 
--------------------

To set the project to use the NRF5 Abstraction module you need to create an **MRT_PLATFORM** symbol with a value of **MRT_NRF5**. If using the nrf5 project template, this can be done by adding the following linke to ``Makefile``: 

.. code::make 

    CFLAGS += MRT_PLATFORM=MRT_NRF5 

