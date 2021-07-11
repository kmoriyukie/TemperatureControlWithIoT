function Remap (value,from,to) {
    return (value - (-50)) / ((100) - (-50)) * (to - from) + from;
}
function generateColor(temp){
    let R = Remap(temp,0,255);
    let B;
    let G;

    B = 255 - Remap(temp,0,255);
    
    G = Remap(temp,0,100);
    return [R,G,B];
    
}



class HomeCanvas extends p5{
    dim = [900, 600];
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
        this.map = new Map(this);
        this.map.mode = "home";

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
    }
    m_setup(){
        this.createCanvas(this.dim[0],this.dim[1], this.WEBGL);
        this.textFont(this.font);

    }

    m_draw(){
        this.background('rgb(255, 247, 235)');
        
        this.push();

        this.ambientLight(50);
        this.ambientMaterial(200);
        this.directionalLight(255, 255, 255, 2.5, -2, -1);
        this.shininess(100);
        this.push();
        this.perspective(Math.PI / 4.0, this.width / this.height, 1, 500);
        this.camera(-100-50, -20, 360, 10, 0, 0, 0, -1, 0);


        this.push();
        this.scale(1.2);
        this.translate(-10,20,0);
        this.BoxStack.draw(this,[generateColor(-layerInfo.layer1.Temperature),generateColor(layerInfo.layer2.Temperature)]);
        this.pop();
        
        this.pop();
        this.pop();
        this.push();
        this.Bubbles[0].draw(this.engine);
        this.Bubbles[1].draw(this.engine);
        
        this.MapTransf = myTransf.translationMatrix([-this.dim[0]/2,-this.dim[1]/2]);
        
        this.translate(-this.dim[0]/2,-this.dim[1]/2,0);
        this.map.draw(this.engine);
        
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
    }
    default_dHit(point){
        this.onClick();
        
        this.map.detect_hitbox([point[0],point[1]]);
        var newP = math.multiply(myTransf.translationMatrix([-this.dim[0]/2,-this.dim[1]/2]),math.matrix([point[0],point[1],1]));
        this.Bubbles[0].detect_hitbox([newP._data[0],newP._data[1]]);
        this.Bubbles[1].detect_hitbox([newP._data[0],newP._data[1]]);
    }
    m_keyPressed(){
        if(this.keyCode == 27){
            curLayer = "none";
            HOME_CANV.map.visible = false;
        }
    }
}

var HOME_CANV = new HomeCanvas();