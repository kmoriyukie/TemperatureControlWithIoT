class IDS_List{
    id_list = [];
    constructor(){
        this.PromiseIDsList();
    }
    
    
    requestList(response){// Request IDS List
        if(response!= undefined){
            console.log("ble");
            response.forEach(element => this.id_list.push(element))
        }
    }
    ResolveIDList(response) {
        if(response!= undefined){
            let json = JSON.parse(response);
            
            json.forEach(element => this.id_list.push(new ListElement(element.local_ID, element.remote_ID, element.Layer)));

            const List_c = document.createElement("ul");
            List_c.style.borderColor = "black";
            List_c.style.float = "center";
            List_c.className += "IdsList";

            const list = document.getElementById("ids_list");
            this.id_list.forEach(element => {    
                this.createItem(element,list);
            });

        }
    }
    PromiseIDsList(){
        const xmlhttp = new XMLHttpRequest;
        const url = 'https://69c7mi9azh.execute-api.eu-west-1.amazonaws.com/GetIDs';

        let obj = this;

        xmlhttp.onload = function(){
            if(this.readyState == 4 && this.status == 200){
                obj.ResolveIDList(this.response);
            }
        }

        xmlhttp.open('GET', url, true);
        xmlhttp.send();
    }
    createItem(element,list){
        const newDiv = document.createElement("div");
        const newLine = document.createElement("div");
        newLine.className = "IdsList_line";
        newLine.appendChild(element.createLineElement("Local ID",element.local_id,false));
        const newBar = document.createElement("div");
        newBar.className = "Line_Element_cBar";
        newLine.appendChild(newBar);
        newLine.appendChild(element.createLineElement("Remote ID",element.remote_id,true));
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
                self.ResolveUpdateRemoteID(JSON.parse(this.response));
            }
        }

        xmlhttp.open('POST', url, true);
        xmlhttp.send(JSON.stringify({local_ID: this.local_id, remote_ID: this.remote_id, Layer: this.Layer}));
    }
    ResolveUpdateRemoteID(response){
        console.log('{ local_ID: '+ response.local_ID.toString() +', remote_ID: '+ response.remote_ID.toString()+ ', Layer: ' + response.Layer.toString() +'}');
        this.inputHandler.value = response.remote_ID;
    }
    createLineElement(txt,id,type){
        const lable = document.createElement("label");
        lable.textContent = txt;
        const input = document.createElement("input");
        input.textContent = id;
        input.type = "text";
        input.required = true;


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
                if (((event.code) == ('Enter')) || ((event.code) == ('NumpadEnter'))){
                    event.preventDefault();
                    obj.remote_id = input.value;
                    obj.PromiseUpdateRemoteID();
                }
            });
        }
        return newElement;
    }
}

function ble(){
    console.log("bleble");
}

