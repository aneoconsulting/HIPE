[TOC]



# <u>How to build HIPE</u>

### 1) For Windows 7 or higher

- Clone from public github : <https://github.com/hipe> or from ANEO internal dev 	<https://gitlab.aneo.fr/hipe-group/hipe>

- Go to the root folder of the project and execute the batch script :

  ```
  compile-ALL.bat
  ```

  The script will download all dependencies once. There is an automatic bootstrap and any error during this step shall be solved before any other step.

     Note: *For any development in your module folder, you only need to call the batch script inside your module folder directly.*

- By default, the batch script compile-ALL.bat It will compile HIPE Core and ANEO vision module. If you want to compile another module, you must run the script corresponding to your module i.e. stage 

- When the compilation is achieved properly. You should have a new install folder in the root folder. Congratulations, you can now open your module project solution in:

   **[Git Clone folder]/source-modules/[module_name]/build/[module_name].sln**



### 2) For Ubuntu 18.04 on X86 64 bits

*For quick usage. You should see the [docker hub](https://cloud.docker.com/u/hipe/repository/list) to download the container of HIPE for you specific machine. It's more convenient to take the container and to avoid to install several dependencies that you wouldn't install or to avoid conflict with your current configuration.* 

[TODO]

- Clone from public github : <https://github.com/hipe> or from ANEO internal dev 	<https://gitlab.aneo.fr/hipe-group/hipe>

- Go to the root folder of the project and execute the batch script :

  ```
  ./compile-ALL.sh
  ```

  . The script will download all dependencies once. There is an automatic bootstrap and any error during this step shall be solved before any other step.

     Note: *For any development in your module folder, you only need to call the batch script inside your module folder directly.*

- By default, the batch script compile-ALL.sh It will compile HIPE Core and ANEO vision module. If you want to compile another module, you must run the script corresponding to your module i.e. stage 

- When the compilation is achieved properly. You should have a new install folder in the root folder.

### 3) For Ubuntu 18.04 on IBM Power 9 + GPU V100

*For* quick usage. You should see the [docker hub](https://cloud.docker.com/u/hipe/repository/list) to download the container of HIPE for you specific machine. It's more convenient to take the container and to avoid to install several dependencies that you wouldn't install or to avoid conflict with your current configuration. 

[TODO]

- Clone from public github : <https://github.com/hipe> or from ANEO internal dev 	<https://gitlab.aneo.fr/hipe-group/hipe>

- Go to the root folder of the project and execute the batch script :

  ```
  ./compile-ALL.sh
  ```

  . The script will download all dependencies once. There is an automatic bootstrap and any error during this step shall be solved before any other step.

     Note: *For any development in your module folder, you only need to call the batch script inside your module folder directly.*

- By default, the batch script compile-ALL.sh It will compile HIPE Core and ANEO vision module. If you want to compile another module, you must run the script corresponding to your module i.e. stage 

- When the compilation is achieved properly. You should have a new install folder in the root folder.

##### 