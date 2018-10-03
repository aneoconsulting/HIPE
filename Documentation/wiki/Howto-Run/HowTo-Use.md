[0]: #content
[2]: #-how-to-use-hipe
[21]: #-json-requests
[211]: #-syntax-of-a-json-request
[2111]: #-usefulness-of-the-requests-name
[2112]: #-processing-chain-and-its-filters
[2113]: #-scheduler
[2114]: #-linking-data-and-supported-data-types
[212]: #-send-requests-to-hipe
[2121]: #-postman
[21211]: #-import-requests
[21212]: #-create-and-edit-a-request
[21213]: #-save-your-request
[21214]: #-send-a-request
[2122]: #-web-interface

[![License: AGPL v3](https://img.shields.io/badge/License-AGPL%20v3-blue.svg)](https://www.gnu.org/licenses/agpl-3.0)
________________________
# Content
1. [How to use HIPE][2]
	* [JSON Requests][21]
		* [Syntax of a JSON request][211]
		* [Send requests to HIPE][212]
________________________

# [[~][0]] How to use HIPE
## [[..][2]] JSON Requests    
* [Syntax of a JSON request][211] 
    * [Usefulness of the request name][2111] 
	* [ Processing chain and its filters][2112] 
	* [Scheduler][2113] 
	* [Linking data and supported data types][2114] 
* [Send requests to HIPE][212]
	* [Postman][2121] 
        * [Import requests][21211] 
		* [Create and edit a request][21212]
		* [Save your request][21213] 
		* [Send a request][21214]
	* [Web Interface][2122]
<br></br><br></br><br></br>


HIPE is a http server.    
The communication method we use to transfer data is to send them using the JSON syntax.    
If you never worked with it, you can quickly learn the basics by reading its documentation on [the w3schools website](https://www.w3schools.com/js/js_json_intro.asp)


### [[..][21]] Syntax of a JSON request    
* [Usefulness of the request name][2111]
* [ Processing chain and its filters][2112] 
* [Scheduler][2113] 
* [Linking data and supported data types][2114] 
<br></br><br></br><br></br>


Here is a request as an example:
```json
{
	"name": "resize",


	"filters": [
		{
			"RootFilter": { 
				"name": "root" 
			} 
		},
		{
			"Resize": {
				"name": "resize",
				"need": ["root"],
				"ratio": 4
			}
		},
		{
			"ShowImage": { 
				"name": "show",
				"need": ["resize"],
				"wait": 1
			}
		}
	],
	
	
	"orchestrator": "DefaultScheduler",
	
	
	"data": {
		"type": "IMGF",
		"path":"C:\\workspace\\data\\images\\image_01.jpg"
	}
}
```
The body of a valid JSON request contains:

* Its name (``name`` field)
* The processing chain (``filters`` array)
* The scheduler symbolizing how the chain's nodes will be processed (``orchestrator`` filed)
* The data which will be processed by the chain (``data`` field)

#### [[..][211]] Usefulness of the request name
The name _is_ the most important information on a request. As your requests graphs will grow, building then processing them will take some time. One of the key concepts of HIPE is to capitalize and reuse the more possible things, _even requests_.    
It is not currently possible to do so but in a future version you will be able to use a saved in database graph just by sending a JSON request containing its name, the desired scheduler, and the data to be processed. The filters array will not be needed anymore.

#### [[..][211]] Processing chain and its filters
The processing chain can be seen as a graph where each node is a different _filter_ which will “do something” on its input data. So basically, a _filter_ can represent an algorithm, like one which will alter an image, or another one which will find and identify an object in a video.

Let’s analyze in detail a filter extracted from the last example:
```json
"Resize": {
	"name": "resize",
	"need": ["root"],
	"ratio": 4
}
```
Each JSON filter node must contain:

* A name, required for its identification (``name`` field)
* A list of all its direct parents nodes’ names (``need`` array)
* If existing, its exposed calibration parameters (in our case the ``ratio`` field)

In the JSON request body, the order on which all the filters are written is not important. You are not forced to do it in their logical processing order, the ``need`` arrays and ``name`` fields will be used by the chosen scheduler to reorder them.

As for the exposed calibration parameters, they are variables exposed in the source code. It permits us to fine tune their behavior without having to each time recompile the whole project again.

**Note:** In the case where the processing chain is saved in HIPE's database, you don't need to write the filters array part in your JSON request. The graph will be found using the request's name.

#### [[..][211]] Scheduler
As said before, the orchestrator is the scheduler which will reorder the processing of the nodes, taking into account their dependencies and the available hardware.     
For now, only the _DefaultScheduler_ model is available, where each filter will be processed sequentially respecting the dependencies order.


#### [[..][211]] Linking data and supported data types
**[TODO]**    
HIPE can work with images, videos (or image sequences), as well as live data streams acquired from hardware like video cameras or RTP (**R**eal-time **T**ransport **P**rotocol).

Here is a table containing the primary data types and their corresponding identifier tags

|   **Type**	|   **Identifier**	|   **Note**	|
|---	|---	|---	|
|   image	|   ``IMGF``	|   path: "*image_path*"	|
|   video	|   ``VIDF``	|   path: "*video_path*"	|
|   webcam	|   ``VIDF``	|   path: "*id_cam"* (i.e. 0,1, ...)"	|
|   image sequence	|   ``SEQIMGD``	|   path: "*images_dir_path*"	|


To link some data to your request, you must add a ``data`` field. As shown in the example, the field is not part of the ``filters`` array.

The field must contain:

* The data type containing the identifier of the data type you want to use (``type`` field)
* The data path (``path`` field)

**Note:** Those data types can also be used as output. To stream the output we use GStreamer and the ``StreamResultFilter`` filter. More information will be added to the API documentation.

### [[..][21]] Send requests to HIPE
The tool we use to edit and send JSON requests to HIPE is Postman. It is not a prerequisite, so you can alternatively use any tool you want.    
Assuming you want to use Postman, you can download it [here](https://www.getpostman.com/).    
You will also find collections of tests requests in the ``/Hipe/tests/data/request/`` folder. They are in the Postman format but it internally uses text, so you can open them in any text editor you want to peek at their content.


#### [[..][212]] Postman
* [Import requests][21211]
* [Create and edit a request][21212]
* [Save your request][21213] 
* [Send a request][21214] 
<br></br><br></br><br></br>


##### [[..][2121]] Import requests
In Postman, every request must be part of a collection. To import a collection, you must simply click on the _import_ button (1.) in the upper left corner of Postman’s interface. The collection will be shown in the left side bar. If the side bar is hidden you can show it by clicking on button _Show/Hide Sidebar_ (2.) next to the _import_ button.


##### [[..][2121]] Create and edit a request
In Postman, all the opened requests are shown in different tabs. By default, if no request is already opened, a new one will be shown and it is possible to directly edit it. But, if at least one request is opened, the default empty one will be hidden and you will need to click on the plus sign button at the right of the last opened request’s tab to create a new one (3.).   
To be able to write the request, you must chose the ``POST`` mode instead of the ``GET`` mode (4.). It will unlock the _Body_ tab. It is here that you will write the content of the request. You will also have to check the _raw_ button to enable text edit, then chose the _JSON (application/json)_ instead of _text_ in the orange drop down menu to enable syntax highlighting.


##### [[..][2121]] Save your request
You can save your request by clicking on the _Save_ button at the right of the request’s tab window (5.). Remember that in Postman, every request must be linked to a collection. If no collection was already created, you will have to create a new one. You can do so by clicking on the _+ Create Collection_ button at the bottom of the opened save dialog.


##### [[..][2121]] Send a request
HIPE’s http server listen on port 9090.    
When used in a debug environment, you will probably deploy the server locally. In that case, the address will be localhost. If deployed on a remote environment, you will have to input the corresponding IP address (6.).    
A complete address will be similar to: ``localhost:9090/json``.    
To send the request you will have to click on the blue _Send_ button (7.). The server will then send its response when the processing will be done. The response is also in JSON format and will be shown just below the body of the sent request.

**Note:** HIPE internally uses the 9090 port to listen for and send JSON requests but It will change in the near future with the apparition of configuration files. You will be able to manually set the port you want to use.

#### [[..][212]] Web Interface
An alternative to postman is our webapp call us for more precision. 