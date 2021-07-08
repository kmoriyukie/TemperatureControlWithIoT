class IDS_List{
    id_list = [];
    constructor(){
        // this.size = 0;
        // this.id_list = [];
    }

    requestList(){// Request IDS List

        this.id_list.push({local_id: 0,remote_id: 0});

        // this.size++;
    }

    initialize(){
        this.requestList();
        console.log(this.id_list);
        // for (const iterator of this.id_list) {
        //     this.createItem(iterator);
        // }
        const newDiv = document.createElement("div");
        this.id_list.forEach(element => {    
            this.createItem(element);
        });
    }


    createItem(ID_pair){
        const newDiv = document.createElement("div");
        const newContent = document.createTextNode("Hi there and greetings!");
        newDiv.appendChild(newContent);
        newDiv.className = "List_item";
        const currentDiv = document.getElementById("ids_list");
        currentDiv.appendChild(newDiv);
    }
}