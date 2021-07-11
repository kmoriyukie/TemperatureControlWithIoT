class my_object{
    pos;
    dim;
    rot;
    scale;
    transf;
    visible;

    type;
    
    parent;
    
    constructor(){
        this.rot = 0;
        this.pos = [0,0];
        this.dim = [0,0];
        this.scale = 1;
        this.transf = math.identity(3);
        this.parent = [];
        this.visible = true;
        this.type = "none";
    }
    detect_hitbox(point){
        //Hitbox
        //Hitbox
        //Hitbox
        //Hitbox
        //Hitbox
        return false;
    }
    default_dHit(point){
        this.onClick();
        // console.log(this.type);
        switch(this.type){
            case "bubble":
                // if(this.label == "22"){
                //     console.table(this.type,point);    
                // }
            // case "walls":
            // case "Map":
            break;
            default:
                // console.table(this.type,point);
            break;
        }
        this.parent.forEach(element => {
            element.detect_hitbox(point);
        });
    }
    transforms(s,r,t){
        this.transf = math.multiply(math.multiply( myTransf.scaleMatrix(s), myTransf.rotationMatrix(r)), myTransf.translationMatrix(t));
    }
    onClick(){

    }
    onPreload(){
        this.parent.forEach(element => {
            element.onPreload();
        });
    }
}

class Walls extends my_object{
    constructor(posX,posY,rot,sc,parent_dim){
        super();
        this.rot = 0;
        this.pos = [posX,posY];
        this.parent_dim = parent_dim;
        this.dim = [parent_dim[0]-2*this.pos[0],parent_dim[1]-2*this.pos[1]];
        this.transf = math.identity(3);

        m = new Mote(0+this.pos[0]/2,this.dim[1]/2,0);
        m.layer = 1;
        m.label = "11"
        m.info.temp = moteInfo.m1.Temperature;
        m.info.humidity = moteInfo.m1.Humidity;
        m.info.airflow = moteInfo.m1.Airflow;
        m.info.battery = moteInfo.m1.Battery;
        this.parent.push(m);
        this.type = "walls";
        this.scale = sc;

        m = new Mote(this.dim[0]-this.pos[0]/2,this.dim[1]/2,0);
        m.layer = 1;
        m.label = "12";
        m.info.temp = moteInfo.m2.Temperature;
        m.info.humidity = moteInfo.m2.Humidity;
        m.info.airflow = moteInfo.m2.Airflow;
        m.info.battery = moteInfo.m2.Battery;
        
        this.parent.push(m);

        var m = new Mote(this.dim[0]/2,3*this.pos[1]/2,0);
        m.layer = 0;
        m.label = "21";
        m.info.temp = moteInfo.m3.Temperature;
        m.info.humidity = moteInfo.m3.Humidity;
        m.info.airflow = moteInfo.m3.Airflow;
        m.info.battery = moteInfo.m3.Battery;
        
        this.parent.push(m);
        
        m = new Mote(this.dim[0]/2,this.dim[1]-3*this.pos[1]/2,0);
        m.layer = 0;
        m.label = "22";
        m.info.temp = moteInfo.m4.Temperature;
        m.info.humidity = moteInfo.m4.Humidity;
        m.info.airflow = moteInfo.m4.Airflow;
        m.info.battery = moteInfo.m4.Battery;
        
        this.parent.push(m);

    }

    detect_hitbox(point){
        if(this.visible){
            let newP = math.multiply(this.transf, math.matrix([point[0],point[1],1]));
            if(newP._data[0] >= 0 && newP._data[0] <= (this.dim[0]*this.scale)){
                if(newP._data[1] >= 0 && newP._data[1] <= (this.dim[1]*this.scale)){
                    this.default_dHit([newP._data[0],newP._data[1]]);
                    return true;
                }
            }
        }
        return false;
    }
    onClick(){
        // console.log("Click on walls");
    }
    draw(engine){
        if(!this.visible) return;
        engine.push();
        engine.scale(this.scale);
        engine.rotate(0);
        engine.noFill();
        // engine.rect(this.pos[0],this.pos[1],200,200);
        engine.translate(this.pos[0],this.pos[1]);
        this.transforms(this.scale,0,[-this.pos[0],-this.pos[1]]);
        
        engine.strokeWeight(1);
        engine.line(0,0,2*this.pos[0],2*this.pos[1]);
        engine.line(0,this.dim[1],2*this.pos[0],-2*this.pos[1]+this.dim[1]);
        engine.line(this.dim[0],0,-2*this.pos[0]+this.dim[0],2*this.pos[1]);
        engine.line(this.dim[0],this.dim[1],-2*this.pos[0]+this.dim[0],-2*this.pos[1]+this.dim[1]);
        engine.strokeWeight(4);
        engine.rect(0,0,this.dim[0],this.dim[1]);

        engine.push();
        engine.translate(this.pos[0]*2,this.pos[1]*2);
        engine.strokeWeight(1);
        engine.rect(0,0,this.dim[0] - this.pos[0]*4,this.dim[1]-this.pos[1]*4);
        engine.pop();

        engine.push();
        engine.translate(this.pos[0],this.pos[1]);
        engine.strokeWeight(1);
        engine.rect(0,0,-2*this.pos[0]+this.dim[0],this.dim[1]-2*this.pos[1]);
        engine.pop();


        // if(this.layer)

        // for(let i = 0; i < 4; i++){
        //     switch(this.layer){
        //         case "none":
        //             this.parent[i].draw(engine);
        //         break;
        //         case "Layer 1":
        //             if(i == 0 || i == 1) this.parent[i].draw(engine);
        //         break;
        //         case "Layer 2":
        //             if(i == 2 || i == 3) this.parent[i].draw(engine);
        //         break;
        //     }
        // }

        this.parent.forEach(element => {
            element.draw(engine);
        });
        engine.pop();

    }
    unselectParents(){
        this.parent.forEach(element => {
            element.unselectParents();
        });
        // console.log("W");
    }
}

class bubble extends my_object{
    rot = 0;
    axis = [0,0,0];


    constructor(pos, size){
        super();
        this.dim = [size,size];
        this.pos = [pos.x, pos.y];
        this.color = '#e28743';
        this.rot = 0;
        this.label = "BLAU";
        this.textColor = '#21130d';
        this.textSize = 32;
        this.type = "bubble";
        this.highlight = '#ff9900';
        this.defaultcolor = '#e28743';
        this.font = "Helvetica"
        this.appendix = false;

        this.visible = false;

    }


    onClick(){
        if(this.mode == "bubhome"){
            // function toggleMap(){
            //     HOME_CANV.map.visible = true;
            // }
            if(this.bubNUM == 0){
                curLayer = "0";
            }
            if(this.bubNUM == 1){
                curLayer = "1";
            }
            toggleMap();
        }
    }
    detect_hitbox(point){
        if(!this.visible) return;


        let newP = math.multiply(this.transf,math.matrix([point[0],point[1],1]));
        let dist = math.sqrt(math.pow(newP._data[0],2)+math.pow(newP._data[1],2));

        if(dist<=this.dim[0]/2){
            this.default_dHit([newP._data[0],newP._data[1]]);
            return true;
        }
        else false;
    }

    draw(engine){
        if(!this.visible) return;
        engine.push();
        engine.translate(this.pos[0],this.pos[1],0.3);
        engine.strokeWeight(1);
        // engine.rect(0,0,50,50);
        engine.fill(this.color);
        engine.stroke(this.color);

        engine.ellipse(0, 0,this.dim[0],this.dim[0]);
        this.transforms(1,0,[-this.pos[0],-this.pos[1]]);
        
        if(this.appendix){
            engine.push()
            const h = this.dim[0]/4.0;
            const w = this.dim[0]/4.0;
            engine.translate(-this.dim[0]/2.5,-this.dim[0]/10);
            engine.triangle(0, 0, -w, h * 0.5, 0, h)
            engine.pop();
        }
        
        engine.fill(this.textColor);
        engine.translate(0,this.textSize/3,5);
        

        engine.textAlign(engine.CENTER);
        engine.textSize(this.textSize);
        engine.text(this.label,0,0);
        engine.pop();
    }


}

class InfoBox extends my_object{
    rot = 0;
    axis = [0,0,0];
    temp = 50;

    humidity = 37;
    airflow = 1;
    battery = 90;
    constructor(pos, size){
        super();
        this.dim = [size,size];
        this.pos = [pos.x, pos.y];
        this.color = '#b6d5f2';
        this.colorSecond = '#81b0db'
        this.rot = 0;
        
        this.textColor = '#21130d';
        this.textSize = 32;
        this.type = "bubble";
        this.highlight = '#ff9900';
        this.defaultcolor = '#e28743';
        this.font = "Helvetica"

        this.align = "right";

    }
    onPreload(){
        this.cold = engine.loadImage('assets/cold.png');
        this.hum = engine.loadImage('assets/humidity.png');
        this.wind = engine.loadImage('assets/wind.png');
        this.batt = engine.loadImage('assets/battery.png');

        this.parent.forEach(element => {
            element.onPreload();
        });
    }

    onClick(){
        // console.log("Ble");
        // if(this.color == this.highlight) this.color = '#e28743';
        // else this.color = this.highlight;
        // this.visible = !this.visible;
    }
    detect_hitbox(point){
        // console.log(this.type)
        if(!this.visible) return;
        let newPoint = math.multiply(this.transf,math.matrix([point[0],point[1],1]));

        let distX = point[0] - newPoint._data[0];
        let distY = point[1] - newPoint._data[1];

        let dist = math.sqrt(math.pow(distX,2)+math.pow(distY,2));

        if(dist<=this.dim[0]/2){
            this.default_dHit(point);
            return true;
        }
        else false;
    }

    draw(engine){
        if(!this.visible) return;
        engine.push();
        
        engine.translate(this.pos[0],this.pos[1]);
        // engine.strokeWeight(1);
        engine.fill(this.color);
        engine.noStroke();
        
        // engine.strokeJoin(engine.ROUND);
        
        engine.square(0, 0,this.dim[0],20);
        engine.push();
        engine.fill(this.colorSecond);
        engine.translate(this.dim[0]*0.05,this.dim[0]*0.05);
        engine.square(0,0, this.dim[0]*0.9,20);
        engine.pop();
        this.transforms(1,this.rot,this.pos);
        
        engine.push()
        const h = this.dim[0]/4.0;
        var w;
        if(this.align == "right"){
            engine.translate(1,this.dim[0]/2.8);
            w = this.dim[0]/4.0;
        }
        else{
            engine.translate(this.dim[0],this.dim[0]/2.8);
            w = - this.dim[0]/4.0;
        }
        engine.triangle(0, 0, -w, h * 0.5, 0, h)
        engine.pop();

        engine.translate(0,this.dim[0]/9,5);

        engine.push();
        engine.translate(40,0,5);

        engine.image(this.cold,0,0,this.dim[0]/6.0,this.dim[0]/6.0);

        engine.translate(0,this.dim[0]/5.0,5);
        engine.image(this.hum,0,0,this.dim[0]/6.0,this.dim[0]/6.0);

        engine.translate(0,this.dim[0]/5.0,5);
        engine.image(this.wind,0,0,this.dim[0]/6.0,this.dim[0]/6.0);

        engine.translate(0,this.dim[0]/5.0,5);
        engine.image(this.batt,0,0,this.dim[0]/6.0,this.dim[0]/6.0);

        engine.pop();

        engine.fill(this.textColor);
        engine.translate(this.dim[0]/2,this.dim[0]/7.5);
        engine.textAlign(engine.LEFT);
        engine.textSize(this.textSize);

        engine.text(this.temp.toString() + "°",0,0)

        engine.translate(0, this.dim[0]/5.0);
        engine.text(this.humidity.toString() + "%",0,0)

        engine.translate(0, this.textSize+10)
        engine.text(this.airflow.toString() + " m/s",0,0)

        engine.translate(0, this.textSize+10)
        engine.text(this.battery.toString() + "%",0,0)

        engine.pop();
    }
}


class Mote extends my_object{
    bubblemode;
    constructor(posX,posY,rot){
        super();
        this.rot = rot;
        this.pos = [posX,posY];
        this.dim = [50,50];
        this.bubblemode = false;
        this.infomode = false;
        this.label = "999";
        this.scale = 1;

        this.balloon = new bubble({x: 1/2*this.dim[0], y: -1/3*this.dim[0]}, 1/2.0*this.dim[0]);
        this.balloon.textSize = 20;
        this.balloon.label = "!";
        this.balloon.visible = this.bubblemode;

        this.info = new InfoBox({x: this.dim[0]+20, y: -105}, 200);

        this.info.temp = 20;
        this.info.humidity =1;
        this.info.airflow = 10;
        this.info.battery = 50;        
        
        this.threshT = 8;
        this.threshH = 4;
        this.threshA = 1;
        this.threshB = 15;

        this.parent.push(this.info);
        this.parent.push(this.balloon);
        this.type = "mote";

        this.textColor = '#21130d';
        this.textSize = 23;

        this.layer = "0";

        this.error = false;
        
    }
    togglebubble(){
        this.bubblemode = !this.bubblemode;
        this.balloon.visible = this.bubblemode;
        this.info.visible = !this.bubblemode & this.infomode;
    }

    checkStatus(){
        if(this.info.temp <= this.threshT && this.info.humidity <= this.threshH && this.info.airflow >= this.threshA && this.info.battery >= this.threshB){
            this.error = false;
        }
        else{ // Sensor error
            this.error = true;
        }
    }
    draw(engine){
        // console.log(this.visible);
        if(curLayer != "none"){
            if(curLayer != this.layer){
                this.visible = false;
            }
            else this.visible = true;
        }
        else{
            this.visible = true;
        }
        if(!this.visible) return;
        // if()
        this.checkStatus();
        if(!this.error ||  HOME_CANV.visible == false){
            if(this.layer == "0"){
                this.color = "#6600ff";
                this.textColor = 'white';
            }
            else this.color = "#3399ff"
        }
        else{
            this.color = "red";
            this.textColor = 'white';
        }

        if(this.label == 11){
            this.info.align = "left";
            this.info.pos[0] = -200 - 65;
        }
        engine.push();        
        engine.scale(this.scale);
        engine.rotate(0);
        engine.translate(this.pos[0],this.pos[1]);
        // this.dim[1]-3*this.pos[1]/2 -this.pos[1]
        this.transforms(this.scale,this.rot,[-this.pos[0],-this.pos[1]]);
        engine.strokeWeight(1);
        engine.fill(this.color);
        engine.ellipse( 0, 0, this.dim[0], this.dim[0]);

        engine.push();
        this.balloon.draw(engine);
        engine.pop();

        engine.fill(this.textColor);
        engine.push();
        engine.translate(0,this.textSize/3,1);

        engine.textAlign(engine.CENTER);
        engine.textSize(this.textSize);
        engine.text(this.label,0,0);

        
        // this.info.draw(engine);
        engine.pop();

        engine.pop();

        

    }
    detect_hitbox(point){
        if(!this.visible) return;
        
        // console.table(this.transf._data);
        let newP = math.multiply(this.transf,math.matrix([point[0],point[1],1]));


        let dist = math.sqrt(math.pow(newP._data[0],2)+math.pow(newP._data[1],2));
        
        this.parent.forEach(element => {
            if(element.type === "bubble") element.detect_hitbox([newP._data[0],newP._data[1]]);
        });
        // console.log(dist);
        // console.log(this.dim[0]);
        if(dist<=(this.dim[0]*this.scale)/2){
            // console.log("csda");
            this.default_dHit([newP._data[0],newP._data[1]]);
            return true;
        }
        else false;
    }
    onClick(){
        console.log("Click on Mote");
        // if(this.)
        // this.engine.unselectParents();
        this.togglebubble();
    }
    unselectParents(){
        this.bubblemode = false;
        this.balloon.visible = false;
        // this.parent.forEach(element => {
        //     element.unselectParents();
        // });
    }
}


class Map extends my_object{
    engine;
    scale;
    walls;
    visible;
    mode;
    layer;

    parent;
    constructor(eng){
        super();
        this.engine = eng;
        this.parent = [];
        this.type = "Map";
        this.visible = true;
        this.mode = "config";
        
        this.dim = [900, 600];
        this.scale = 1;
        this.walls = new Walls(75,50,0,this.scale, this.dim);
        this.layer = "none";
        this.walls.layer = this.layer;
        // this.info = new InfoBox({x: 0, y: 0}, 200);
        // this.info.testSize = 32;
        // this.balloon = new bubble({x: 0,y: 0},100);// new bubble({x: 0,y: 0},100), new bubble({x: 0,y: 0},100), new bubble({x: 0,y: 0},100)];

        this.parent.push(this.walls);
        // this.parent.push(this.balloon);
        // this.mote = this.addMote();

        // this.parent.push(this.info);
        this.transf = math.identity(3);
        
        this.parent.push()
        
    }
    draw(){
        if(!this.visible) return;

        this.engine.push();
        if(this.mode == "home"){
            // this.engine.background(255);
            this.engine.background('rgb(255, 247, 235)');
            this.engine.translate(0,0,0.5);
        }
        
        this.engine.scale(this.scale);
        this.transforms(this.scale,0,[0,0]);
        // this.engine.rect(0,0,this.dim[0],this.dim[1]);
        this.parent.forEach(element => {
            element.draw(this.engine);
        });
        this.engine.pop();

    }
    onClick(){
        // console.log("Click on Map");
        this.unselectParents();
    }
    unselectParents(){
        this.parent.forEach(element => {
            element.unselectParents();
        });
    }
    transforms(s,r,t){
        this.transf = math.multiply(math.multiply( myTransf.scaleMatrix(s), myTransf.rotationMatrix(r)), myTransf.translationMatrix(t));
    }
    detect_hitbox(point){
        if(this.visible){
            // let newTransf = math.multiply(transform,this.transf);
            // console.table(point);
            let newP = math.multiply(this.transf, math.matrix([point[0],point[1],1]));
            // this.engine.point(newP._data[0],newP._data[1]);
            if(newP._data[0] >= 0 && newP._data[0] <= (this.dim[0]*this.scale)){
                if(newP._data[1] >= 0 && newP._data[1] <= (this.dim[1]*this.scale)){
                    this.default_dHit([newP._data[0],newP._data[1]]);
                    return true;
                }
            }
        }
        return false;
    }
}

function toggleMap(){
    HOME_CANV.map.visible = true;
}

curLayer = "none";