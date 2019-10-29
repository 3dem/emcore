Overview
========

What are datavis and emvis?
---------------------------

``datavis`` is a Python library that provides basic components for implementing graphical applications
to visualize and analyze scientific data. It has been designed to deal with images data and tabular
information. The library is built around two key concepts:

* **Models**: classes in this category are responsible of reading the different type of data
  (images, volumes, tables) in different formats (xml, SQL, text files, etc). Models will provide
  an uniform interface for Views to display the underlying data.

* **Views**: graphical components that will display the data, fetched from one or more models.

``emvis`` is an extension of ``datavis`` that implements models specific to CryoEM using the ``emcore``
library. It also provides some extended views and many utility functions to handle CryoEM data.

Installation
------------
The preferred way to install these libraries is via conda::

    conda install -c emforge emcore datavis emvis


Another way is to use `Python package index`_::

    pip install emcore datavis emvis  # Currently emcore is not available via PyPI


The source code (including the tests and examples) can be found on GitHub:

* https://github.com/3dem/datavis.git
* https://github.com/3dem/emcore.git
* https://github.com/3dem/emvis.git

Citing
------

If you find ``emcore``, ``datavis`` or ``emvis`` useful in your work, please cite:

De la Rosa-Trevin, J.M., Hernandez Viga, P., ... (2019) Developing basic building blocks...
. *Acta Cryst.* D\ **73**:469--477.
`doi: 10.1107/S2059798317007859`_


Contributing
------------

Please use the GitHub repositories for bug reports and feature requests, or
`email CCP-EM`_.

Code contributions are very welcome, please fork the corresponding repository and
submit pull requests with your proposed changes.


Licence
-------

The project is released under the `GPLv3`_ licence.


.. _Python package index: https://pypi.org/project/mrcfile
.. _email CCP-EM: ccpem@stfc.ac.uk
.. _`doi: 10.1107/S2059798317007859`: https://doi.org/10.1107/S2059798317007859
.. _GPLv3: https://www.gnu.org/licenses/gpl-3.0.en.html