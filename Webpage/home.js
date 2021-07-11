// temp -50 -> 100

function Remap (value,from,to) {
    return (value - (-50)) / ((100) - (-50)) * (to - from) + from;
}

function generateColor(temp){
    
    let R = Remap(temp,0,255);
    let B;
    let G;

    B = 255 - Remap(temp,60,140);
    
    G = Remap(temp,0,120);
    return [R,G,B];
    
}



class HomeCanvas extends p5{
    dim = [900, 600];
    // colors = ['#3375FF', '#3396FF'];
    parent;
    sc;
    constructor(){
        super(sketch, window.document.getElementById("HomeCanvas"));
        this.setup = this.m_setup;
        this.draw = this.m_draw;
        this.mouseClicked = this.m_mouseclick;
        this.keyPressed = this.m_keyPressed;
        this.engine = this;
        this.parent = [];
        this.AddBox = function(pos){
            return{
                draw(engine,colors){
                    engine.translate(pos.x, pos.y, pos.z);
                    engine.fill(colors);
                    engine.box(150,50,300);
                }
            }
        }
        
        this.Boxes = [this.AddBox({x: 0, y: 0, z: 0}), this.AddBox({x: 0,y: -55, z: 0})]
        
        this.BoxStack = {
            size: 1,
            pos: {x: 0, y: 0, z: 0},
            rot: {theta: 0, axis: [0, 0, 0]},
            // colors: ['#3375FF', '#3396FF'],
            draw(engine,colors){
                engine.push();
                engine.normalMaterial();
                engine.translate(this.pos.x, this.pos.y, this.pos.z);
                engine.scale(this.size);
                engine.stroke(51);
                for(let i = 0; i < 2; i++){
                    engine.Boxes[i].draw(engine,colors[i]);
                }
                engine.pop();
            }
        }

        this.info1 = new InfoBox({x: -15, y: -12}, 200);
        this.info1 = new InfoBox({x: -15, y: - 12 - 55}, 200);

        this.Bubbles = [new bubble({x: 270, y: 70}, 100), new bubble({x: 270, y: -40}, 100)];
        let j = 1;
        this.Bubbles.forEach(element => {
            element.appendix = true;
            element.visible = true;
            element.mode = "bubhome";
            element.bubNUM = j;
            j--;
        });
        // this.parent.push(new Map(this));
        this.map = new Map(this);
        this.map.mode = "home";
        // this.map.pos = [this.dim[0]/2,this.dim[1]/2];

        this.transf = math.identity(3);
        this.visible = true;
        this.sc = 1;
        this.map.visible = false;
        let b = -50;
        this.a = b;
        this.BubbleT = [];

        this.Bubbles[0].label = "L1";
        this.Bubbles[1].label = "L2";
        
        this.threshT = 1;
        this.onPreload();
        
    }
    onPreload(){
        this.font = this.loadFont(
            "assets/Staatliches-Regular.ttf"
        );
        
        this.map.onPreload();
        // this.parent.forEach(element => {
        //     element.onPreload();
        // });
    }
    m_setup(){
        // this.createCanvas(3*this.displayWidth/5, 4*this.displayHeight/6, this.WEBGL);
        this.createCanvas(this.dim[0],this.dim[1], this.WEBGL);
        this.textFont(this.font);

    }

    m_draw(){
        this.background('rgb(255, 247, 235)');
        // this.background('rgb(255, 255, 255)');
        // this.background('#3d3838');

        
        
        
        this.push();

        this.ambientLight(50);
        this.ambientMaterial(200);
        // this.specularMaterial(80);
        // this.lights();
        this.directionalLight(255, 255, 255, 2.5, -2, -1);
        this.shininess(100);
        this.push();
        this.perspective(Math.PI / 4.0, this.width / this.height, 1, 500);
        this.camera(-100-50, -20, 360, 10, 0, 0, 0, -1, 0);


        this.push();
        this.scale(1.2);
        this.translate(-10,20,0);
        // this.a++;
        // if(this.a>100) this.a = -50;

        // this.BoxStack.draw(this,[generateColor(l1),generateColor(255-this.a)]);
        this.BoxStack.draw(this,[generateColor(-layerInfo.layer1.Temperature),generateColor(layerInfo.layer2.Temperature)]);
        this.pop();
        
        // this.translate(0,0,300);
        // this.scale(0.1);
        // this.rotate(this.PI, [0,0,1]);
        // this.pop();

        // this.push();
        
        // this.rotateY(-math.PI/12);
        this.pop();
        this.pop();
        this.push();
        // this.translate(120,-70,0);
        // this.BubbleT[0] = myTransf.translationMatrix([0,0]);
        this.Bubbles[0].draw(this.engine);
        this.Bubbles[1].draw(this.engine);
        // this.translate(0,30,0);
        // this.Bubbles.forEach(element =>{
        //     element.draw(this.engine);
        //     // this.translate(0,-55,0);
        // });
        this.MapTransf = myTransf.translationMatrix([-this.dim[0]/2,-this.dim[1]/2]);
        // this.MapTransf = myTransf.translationMatrix([-200+75,-100+40]);
        // this.scale(0.5);
        this.translate(-this.dim[0]/2,-this.dim[1]/2,0);
        this.map.draw(this.engine);
        // this.map.mode = "home";


        // this.mape.draw();
        this.pop();
    }

    m_mouseclick(){
        if(!this.visible) return;
        this.detect_hitbox([this.mouseX,this.mouseY]);
    }
    detect_hitbox(point){
        if(this.visible){
            if(point[0] >= 0 && point[0] <= (this.dim[0]*this.sc)){
                if(point[1] >= 0 && point[1] <= (this.dim[1]*this.sc)){
                    this.default_dHit(point);
                    return true;
                }
            }
        }
        return false;
    }
    onClick(){
        // console.log("Click Home");
    }
    default_dHit(point){
        this.onClick();
        // let newP = math.multiply(this.MapTransf, math.matrix([point[0],point[1],1]));
        // this.map.detect_hitbox([newP._data[0],newP._data[1]]);
        // console.table([newP._data[0],newP._data[1]]);
        // console.table(point);
        // 200/100
        
        this.map.detect_hitbox([point[0],point[1]]);
        // console.table(this.BubbleT[0]._data);
        var newP = math.multiply(myTransf.translationMatrix([-this.dim[0]/2,-this.dim[1]/2]),math.matrix([point[0],point[1],1]));
        this.Bubbles[0].detect_hitbox([newP._data[0],newP._data[1]]);
        this.Bubbles[1].detect_hitbox([newP._data[0],newP._data[1]]);
        // for(let i = 0; i <= 2; i++){
        //     BubbleT
        //     this.Bubbles[i].detect_hitbox([point[0],point[1]]);
        // }
        // this.Bubbles.forEach(element => {
        //     element.detect_hitbox([point[0],point[1]]);
        // });
        // this.parent.forEach(element => {
        //     switch(element.type){
        //         case "map":
        //             let newP = math.multiply(this.MapTransf, math.matrix([point[0],point[1],1]));
        //             // console.log(point);
        //             // console.log(newP._data);
        //             element.detect_hitbox([newP._data[0],newP._data[1]]);
        //         break;
        //     }
        // });
    }
    m_keyPressed(){
        if(this.keyCode == 27){
            curLayer = "none";
            HOME_CANV.map.visible = false;
        }
        // console.log(this.keyCode);
    }
}

var HOME_CANV = new HomeCanvas();