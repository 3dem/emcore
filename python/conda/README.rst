Notes to build Anaconda binaries
================================


Installing emcore into a given conda environment:
-------------------------------------------------

.. code-block:: bash

    conda create -y --name emc37 emcore -c emforge
    conda activate emc37

    # Run some tests

    python -m emcore.tests


Development
~~~~~~~~~~~

Manual building in a conda environment
--------------------------------------

If you are a `emcore` developer, you might find useful to build and install the library
into an given conda environment.

First, create a new environment (e.g for Python 3.7) and activate it.
Then run the installation script with the new Python interpreter.

.. code-block:: bash

    cd emcore

    conda create -y --name emc37-dev python=3.7 numpy scikit-build -c conda-forge
    conda activate emc37-dev
    python python/conda/install_to_env.py

    # When installing this way, we need to set the LD_LIBRARY_PATH to the environment lib folder
    # as in the following example
    export LD_LIBRARY_PATH=/home/josem/installs/devel/miniconda3/envs/emc37-dev/lib:$LD_LIBRARY_PATH

    python -m emcore.tests


Creating binaries with conda-build
----------------------------------

You will need a conda enviroment with conda-build installed, then execute the recipe:

.. code-block:: bash

    cd emcore

    conda create -y --name emc-conda-build conda-build -c conda-forge anaconda-client
    conda activate emc-conda-build
    conda build python/conda/recipe -c conda-forge

    # Then upload the binaries with:
    anaconda upload /home/josem/installs/devel/miniconda3/conda-bld/linux-64/emcore-0.0.5-py37ha8d69ae_0.tar.bz2 -u emforge

    # Do the same with binaries for other Python versions (py35, py36 and py38)
    # Remember to use -u emforge as the user to upload

