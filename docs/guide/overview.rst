Overview
========

Architecture
------------

``emcore`` is a basic scientific library for data processing in CryoEM written in C++11 and with Python bindings.
The library contains submodules for basic mathematical operations and operating system specific functions.
Another submodule provides the bread and butter for basic CryoEM data handling: Images and Tables.
Based on these submodules, there is a processing submodule that implements
basic operations such as filters, alignment, Fourier transforms,

``emcore`` is implemented in C++11, exploiting some of new features of this version of the language
for more efficient memory manipulation and more functions already built in the standard library.
Additionally, we use `pybind11`_ to generate a binding layer to the existing classes and methods,
that expose most of the available functionality to Python.

The general architecture, as well as many elements on the design and implementation of the main components,
has been somehow inspired and influenced by existing code in established packages such as
Xmipp \cite{Sorzano2004, Scheres2008, DeLaRosa2013}, Eman2 \cite{Tang2007}, Bsoft \cite{Heymann2007} and
Relion \cite{Scheres2012a, Kimanius2016, Zivanov2018}, among others.

The ``emvis`` library extends ``datavis`` by using ``emcore`` to implement specific data models and views.
The general architecture is shown in the following figure:

.. image:: /images/architecture.png
    :width: 600px
    :align: center


Installation
------------
The preferred way to install the libraries is via conda::

    conda install -c emforge emcore


Source code and instructions about how to build the library from sources
can be found at the GitHub repository:

* https://github.com/3dem/emcore.git


Citing
------

If you find ``emcore``, ``datavis`` or ``emvis`` useful in your work, please cite:

De la Rosa-Trevin, J.M., Hernandez Viga, P.A., Oton, J. and Lindalh, E.
*Development of basic building blocks for Cryo-EM :emcore and emvis software libraries*.
*Acta Cryst.* D\ **73**:469--477. 2019


Contributing
------------

Please use the GitHub repositories for bug reports and feature requests, or
`email CCP-EM`_.

Code contributions are very welcome, please fork the corresponding repository and
submit pull requests with your proposed changes.


Licence
-------

The project is released under the `GPLv3`_ licence.


.. _email CCP-EM: ccpem@stfc.ac.uk
.. _pybind11: https://github.com/pybind/pybind11
.. _GPLv3: https://www.gnu.org/licenses/gpl-3.0.en.html