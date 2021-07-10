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
    constructor(posX,posY,rot){
        super();
        this.rot = rot;
        this.pos = [posX,posY];
        this.mdim = [150,100];
        this.transl = [-150,-100];
        this.dim = [900+this.transl[0]-this.mdim[0],600+this.transl[1]-this.mdim[1]];
        this.transf = math.identity(3);
        var m = new Mote(this.dim[0]/2,this.dim[1]-this.mdim[0]/4+12,0);
        m.layer = 0;
        m.label = "21";
        this.parent.push(m);
        
        m = new Mote(this.dim[0]/2,this.mdim[0]/4-12,0);
        m.layer = 0;
        m.label = "22";
        m.info.temp = 6;
        this.parent.push(m);
        
        m = new Mote(this.dim[0]+this.mdim[0]/4,this.dim[1]/2,0);
        m.layer = 1;
        m.label = "11";
        m.info.temp = 6;
        this.parent.push(m);
        m = new Mote(-this.mdim[0]/4,this.dim[1]/2,0);
        m.layer = 1;
        m.label = "12"
        m.info.temp = 6;
        this.parent.push(m);
        this.type = "walls";
    }

    detect_hitbox(point){
        if(!this.visible) return;
        let newTransf = math.multiply(this.transf,math.matrix([point[0],point[1],1]));
        return this.default_dHit(point);
    }
    draw(engine){
        if(!this.visible) return;
        engine.push();
        // engine.scale()
        engine.rotate(this.rot);
        engine.noFill();
        engine.translate(this.pos[0]-this.transl[0]/2,this.pos[1]-this.transl[1]/2);
        engine.strokeWeight(1);
        engine.line(0,0,this.pos[0]+this.mdim[0],this.pos[1]+this.mdim[1]);
        engine.line(0,this.dim[1]+this.mdim[1],this.pos[0]+this.mdim[0],this.pos[1]+this.dim[1]);
        engine.line(this.dim[0]+this.mdim[0],0,this.pos[0]+this.dim[0],this.pos[1]+this.mdim[1]);
        engine.line(this.dim[0]+this.mdim[0],this.dim[1]+this.mdim[1],this.pos[0]+this.dim[0],this.pos[1]+this.dim[1]);
        engine.strokeWeight(4);
        engine.rect(0,0,this.dim[0]+this.mdim[0],this.dim[1]+this.mdim[1]);
        engine.translate(this.pos[0]+this.mdim[0]/2,this.pos[1]+this.mdim[1]/2);
        let temp = [this.pos[0]+this.mdim[0]/2-this.transl[0]/2,this.pos[1]+this.mdim[1]/2-this.transl[1]/2];
        this.transforms(1,this.rot,temp);
        engine.strokeWeight(1);
        engine.rect(0,0,this.dim[0],this.dim[1]);
        engine.push();
        engine.translate(this.pos[0]+this.mdim[0]/2,this.pos[1]+this.mdim[1]/2);
        engine.strokeWeight(1);
        engine.rect(0,0,this.dim[0]-this.mdim[0],this.dim[1]-this.mdim[1]);
        engine.pop();

        this.parent.forEach(element => {
            element.draw(engine);
        });
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

    }


    onClick(){
        // console.log("Ble");
        if(this.color == this.highlight) this.color = '#e28743';
        else this.color = this.highlight;
        // this.visible = !this.visible;
    }
    detect_hitbox(point){
        // console.log(this.type)
        if(!this.visible) return;
        let newP = math.multiply(this.transf,math.matrix([point[0],point[1],1]));

        let distX = point[0] - newP._data[0];
        let distY = point[1] - newP._data[1];

        let dist = math.sqrt(math.pow(distX,2)+math.pow(distY,2));

        if(dist<=this.dim[0]/2){
            this.default_dHit([newP._data[0],newP._data[1]]);
            return true;
        }
        else false;
    }

    draw(engine){
        if(!this.visible) return;
        engine.push();
        
        engine.translate(this.pos[0],this.pos[1]);
        engine.strokeWeight(1);
        engine.fill(this.color);
        engine.stroke(this.color);
        engine.ellipse(0, 0,this.dim[0],this.dim[0]);
        this.transforms(1,this.rot,this.pos);
        
        engine.push()
        const h = this.dim[0]/4.0;
        const w = this.dim[0]/4.0;
        engine.translate(-this.dim[0]/2.5,-this.dim[0]/10);
        engine.triangle(0, 0, -w, h * 0.5, 0, h)
        engine.pop();
        
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
        const w = this.dim[0]/4.0;
        engine.translate(0,this.dim[0]/2.8);
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
        this.label = "999";
        this.scale = 1;
        
        this.balloon = new bubble({x: 50, y: 0}, 50);
        this.balloon.textSize = 20;
        this.balloon.label = "999";
        this.balloon.visible = this.bubblemode;

        this.info = new InfoBox({x: 0, y: 0}, 200);

        this.info.temp = 20;
        this.info.humidity = 5;
        this.info.airflow = 3;
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
    }
    togglebubble(){
        this.bubblemode = !this.bubblemode;
        this.balloon.visible = this.bubblemode;
    }
    draw(engine){
        if(this.info.temp <= this.threshT || this.info.hum <= this.threshH || this.info.air >= this.threshA || this.info.battery >= this.threshB){
            if(this.layer == "0"){
                this.color = "#6600ff";
                this.textColor = 'white';
            }
            else this.color = "#3399ff"
        }
        else{ // Sensor error
            this.color = 'red';
            this.textColor = 'white';
        }
        if(!this.visible) return;
        engine.push();        
        engine.rotate(this.rot);
        engine.translate(this.pos[0],this.pos[1]);
        this.transforms(1,this.rot,this.pos);
        engine.strokeWeight(1);
        engine.fill(this.color);
        engine.ellipse( 0, 0, this.dim[0], this.dim[0]);

        engine.push();
        engine.translate(this.dim[0]/10, 0);
        this.balloon.draw(engine);
        engine.pop();

        engine.fill(this.textColor);
        engine.translate(0,this.textSize/3,1);
        engine.textAlign(engine.CENTER);
        engine.textSize(this.textSize);
        engine.text(this.label,0,0);

        
        engine.push();
        engine.translate(this.dim[0],-100);
        this.info.draw(engine);
        engine.pop();

        engine.pop();

        

    }
    detect_hitbox(point){
        if(!this.visible || typeof transform == undefined || typeof this.transf == undefined) return;
        let newP = math.multiply(this.transf,math.matrix([point[0],point[1],1]));
        let distX = point[0] - newP._data[0];
        let distY = point[1] - newP._data[1];

        let dist = math.sqrt(math.pow(distX,2)+math.pow(distY,2));
        
        this.parent.forEach(element => {
            if(element.type === "bubble") element.detect_hitbox([newP._data[0],newP._data[1]]);
        });

        if(dist<=(this.dim[0]*this.scale)/2){
            this.default_dHit([newP._data[0],newP._data[1]]);
            return true;
        }
        else false;
    }
    onClick(){
        // console.log("bleble");
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
    // pos;
    // dim;
    walls;
    visible;

    parent;
    constructor(eng){
        super();
        this.engine = eng;
        this.parent = [];
        this.type = "Map";
        this.visible = true;
        this.walls = new Walls(0,0,0);
        // this.info = new InfoBox({x: 0, y: 0}, 200);
        this.dim = [900, 600];
        // this.info.testSize = 32;
        // this.balloon = new bubble({x: 0,y: 0},100);// new bubble({x: 0,y: 0},100), new bubble({x: 0,y: 0},100), new bubble({x: 0,y: 0},100)];

        this.parent.push(this.walls);
        // this.parent.push(this.balloon);
        // this.mote = this.addMote();

        // this.parent.push(this.info);
        this.scale = 1;
        this.transf = math.identity(3);
        
        this.parent.push()
        
    }
    draw(){
        if(!this.visible) return;
        // this.engine.background(255);

        this.engine.push();

        
        this.engine.scale(this.scale);
        // engine.translate(engine.width * 0.2, engine.height * 0.2);
        this.transforms(this.scale,0,[0,0]);
        this.parent.forEach(element => {
            element.draw(this.engine);
        });
        // engine.strokeWeight(20);
        // this.engine.point(50,50);
        this.engine.pop();

    }
    onClick(){
        console.log("Click on Map");
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
    default_dHit(point){
        this.onClick();
        this.parent.forEach(element => {
            element.detect_hitbox(point);
        });
    }
}
