class IDS_List{
    id_list = [];
    constructor(){
        this.PromiseIDsList();
    }
    
    
    requestList(response){// Request IDS List
        
        // this.id_list.push(new ListElement(0,0));
        // this.id_list.push(new ListElement(0,0));
        // this.id_list.push(new ListElement(0,0));

        if(response!= undefined){
            console.log("ble");
            response.forEach(element => this.id_list.push(element))
        }
        // this.size++;
    }
    ResolveIDList(response) {
        if(response!= undefined){
            let json = JSON.parse(response);
            
            json.forEach(element => this.id_list.push(new ListElement(element.local_ID, element.remote_ID, element.Layer)));

            const List_c = document.createElement("ul");
            List_c.style.borderColor = "black";
            // List_l.style.borderRightStyle = "solid";
            // List_l.style.borderWidth = "thin";
            List_c.style.float = "center";
            List_c.className += "IdsList";
            // List_l.style.textAlign = "right";
            // const List_r = document.createElement("ul");
            // List_r.style.borderColor = "black";
            // List_r.style.borderLeftStyle = "solid";
            // List_r.style.borderWidth = "thin";
            // List_l.style.float = "left";
            // List_r.style.textAlign = "left";



            // const currentDiv = document.getElementById("CONFIG");
            // currentDiv.appendChild(List_c);
            // // currentDiv.appendChild(List_r);
            // this.id_list.forEach(element => {    
            //     this.createItem(element,List_c);
            // });

            const list = document.getElementById("ids_list");
            // currentDiv.appendChild(List_c);
            // currentDiv.appendChild(List_r);
            this.id_list.forEach(element => {    
                this.createItem(element,list);
            });

            // ids_list
        }
    }
    PromiseIDsList(){
        const xmlhttp = new XMLHttpRequest;
        const url = 'https://69c7mi9azh.execute-api.eu-west-1.amazonaws.com/GetIDs';

        let obj = this;

        xmlhttp.onload = function(){
            if(this.readyState == 4 && this.status == 200){
                // console.log(this.response);
                obj.ResolveIDList(this.response);
            }
        }

        xmlhttp.open('GET', url, true);
        xmlhttp.send();
    }
    createItem(element,list){
        const newDiv = document.createElement("div");
        // <form action="/action_page.php">
        //     <label for="fname">First name:</label>
        //     <input type="text" id="fname" name="fname"><br><br>
        //     <label for="lname">Last name:</label>
        //     <input type="text" id="lname" name="lname"><br><br>
        //     <input type="submit" value="Submit">
        // </form>
        
        
        // const lable_R = document.createElement("label");
        // lable_R.textContent = "Remote ID";
        // const input_R = document.createElement("input");
        // input_R.textContent = ID_pair.remote_id;
        
        const newLine = document.createElement("div");
        newLine.className = "IdsList_line";
        newLine.appendChild(element.createLineElement("Local ID",element.local_id,false));
        const newBar = document.createElement("div");
        newBar.className = "Line_Element_cBar";
        newLine.appendChild(newBar);
        newLine.appendChild(element.createLineElement("Remote ID",element.remote_id,true));

        // const newContent = document.createTextNode("Hi there and greetings!");
        // const currentDiv = document.getElementById("ids_list");
        list.appendChild(newLine);
    }
}

class ListElement{
    local_id;
    remote_id;
    constructor(l_id,r_id){
        this.local_id = l_id;
        this.remote_id = r_id;
    }
    PromiseUpdateRemoteID(){
        const xmlhttp = new XMLHttpRequest;
        const url = 'https://69c7mi9azh.execute-api.eu-west-1.amazonaws.com/UpdateIDs';

        const self = this;
        xmlhttp.onload = function(){
            if(this.readyState == 4 && this.status == 200){
                // console.log(this.response)
                self.ResolveUpdateRemoteID(JSON.parse(this.response));
            }
        }

        xmlhttp.open('POST', url, true);
        xmlhttp.send(JSON.stringify({local_ID: this.local_id, remote_ID: this.remote_id, Layer: this.Layer}));
        // return false;
    }
    ResolveUpdateRemoteID(response){
        console.log('{ local_ID: '+ response.local_ID.toString() +', remote_ID: '+ response.remote_ID.toString()+ ', Layer: ' + response.Layer.toString() +'}');
        this.inputHandler.value = response.remote_ID;
    }
    createLineElement(txt,id,type){
        const lable = document.createElement("label");
        lable.textContent = txt;
        // lable.className = "Line_Element_label";
        const input = document.createElement("input");
        input.textContent = id;
        input.type = "text";
        input.required = true;
        // input.value = 

        const newElement = document.createElement("div");
        newElement.appendChild(lable);
        newElement.appendChild(input);
        newElement.className = "Line_Element";

        if(!type){
            newElement.style.alignItems = "Right";
            input.name = "local_id";
            input.readOnly = true;
            input.disabled = true;
            input.value = this.local_id;
            // newElement.style.borderRightStyle = "solid";
            // newElement.style.borderWidth = "thin";
        }
        else{
            newElement.style.alignItems = "Left";
            input.name = "remote_id";
            input.readOnly = false;
            input.value = this.remote_id;
            if(this.local_id == 0) input.disabled = true;
            input.type = "Number";
            this.inputHandler = input;
            const obj = this;
            input.addEventListener('keyup', function(event){
                // console.log(event.code);
                if (((event.code) == ('Enter')) || ((event.code) == ('NumpadEnter'))){
                    event.preventDefault();
                    obj.remote_id = input.value;
                    obj.PromiseUpdateRemoteID();
                    // console.log("ble");
                    // newElement.submit();
                    // document.querySelector('form').submit();
                }
            });
            // newElement.reloa

            // newElement.action = "return false;";
            // newElement.onsubmit = "";//this.PromiseUpdateRemoteID(this);



            // newElement.style.borderleftStyle = "solid";
            // newElement.style.borderWidth = "thin";
        }
        // newElement.appendChild(newBar);
        return newElement;
    }
}

function ble(){
    console.log("bleble");
}

