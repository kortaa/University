package com.mygdx.game;

import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.graphics.Texture;
import com.badlogic.gdx.graphics.g2d.BitmapFont;
import com.badlogic.gdx.graphics.g2d.SpriteBatch;
import com.badlogic.gdx.graphics.g2d.TextureRegion;
import com.badlogic.gdx.math.Rectangle;
import com.badlogic.gdx.scenes.scene2d.ui.Skin;
import com.badlogic.gdx.utils.Array;

import java.util.Iterator;
import java.util.Random;

public class GameMap {
    public Array<Enemy> enemies;
    public Array<Bullet> bullets;
    private Array<Item> items;
    public Player player;
    public Weapon weapon;
    private Bullet bullet;
    private Item item;
    private Enemy enemy;
    private Wave wave;
    private TextureRegion textureRegion;
    private final float mapSizeWidth = 2280;
    private final float mapSizeHeight = 1528;
    private final Rectangle border;
    private int score;



    public GameMap(Player player){
        createTextureRegion("background1.png");
        border = new Rectangle(0,0, mapSizeWidth, mapSizeHeight);

        wave = new Wave(this);

        this.player = player;

        bullets = new Array<>();

        weapon = new Weapon(player, 0.1f);

        enemies = new Array<>();

        items = new Array<>();

        score = 0;

    }
    public void update(){
        wave.update();

        checkHit();
        playerHit();
        collectItems();

        for (Enemy enemy : enemies){
            enemy.update(player);
        }

        for (Item item : items){
            item.update(player);
        }

        weapon.update(enemies);

        if(movingEnemies() > 0 && weapon.isShooting()){
            bullets.add(new Bullet(weapon.getTarget(enemies), 1000, weapon));
        }

        for (Iterator<Bullet> iterBullet = bullets.iterator(); iterBullet.hasNext();){
            bullet = iterBullet.next();
            if(bullet.getBody().overlaps(border)) bullet.update();
            else iterBullet.remove();
        }
    }
    public int movingEnemies(){
        if(enemies.isEmpty()) return 0;
        int num = 0;
        for(Enemy enemy : enemies){
            if(!enemy.isSpawning) num+=1;
        }
        return num;
    }

    public void checkHit(){
        Random random = new Random();
        for(Iterator<Bullet> iterBullet = bullets.iterator(); iterBullet.hasNext();){
             bullet = iterBullet.next();
            for(Iterator<Enemy> iterEnemy = enemies.iterator(); iterEnemy.hasNext();){
                enemy = iterEnemy.next();
                if(bullet.getBody().overlaps(enemy.getBody()) && !enemy.isSpawning)
                {
                    enemy.setHp(enemy.getHp() - bullet.getDamage());
                    iterBullet.remove();

                    if(enemy.isDead()){
                        items.add(new Experience(enemy.getX(), enemy.getY(), "exp.png"));
                        spawnItems(enemy, random);
                        iterEnemy.remove();
                    }
                    break;
                }
            }
        }
    }
    public void spawnItems(Enemy enemy, Random random){
        int life = random.nextInt() % 100;
        int exp = random.nextInt() % 100;

        float x = enemy.getX() + random.nextInt() % 80 - 80;
        float y = enemy.getY() + random.nextInt() % 80 - 80;

        if(x < 0) x=0;
        else if (x + 32 > getMapSizeWidth()) x = getMapSizeWidth() - 32;
        else if (y < 0) y = 0;
        else if (y + 32 > getMapSizeHeight()) x = getMapSizeHeight() - 32;

        if(life > 90) items.add(new Life(x, y, "life.png"));

        int i;
        if(exp <= 60) i = 1;
        else if (exp <= 90) i = 2;
        else i = 3;

        while(i > 0){
            x = enemy.getX() + random.nextInt() % 100 - 50;
            y = enemy.getY() + random.nextInt() % 100 - 50;

            if(x < 0) x=0;
            else if (x + 32 > getMapSizeWidth()) x = getMapSizeWidth() - 32;
            else if (y < 0) y = 0;
            else if (y + 32 > getMapSizeHeight()) x = getMapSizeHeight() - 32;

            items.add(new Experience(x, y, "exp.png"));
            i--;
        }
    }

    public void playerHit(){
        for(Enemy enemy : enemies){
            if(enemy.getBody().overlaps(player.getBody())) player.setHp(player.getHp() - enemy.getDamage());
            if(player.getHp() < 0) player.setHp(0);
        }
    }

    public void collectItems(){
        for(Iterator<Item> iterator = items.iterator(); iterator.hasNext();){
            item = iterator.next();
            if(item.getBody().overlaps(player.getBody())){
                item.addVal(player);
                iterator.remove();
                score += 10;
            }
        }
    }

    public void draw(SpriteBatch batch){
        batch.begin();
        batch.draw(textureRegion, 0, 0);
        batch.end();

        weapon.draw(batch, weapon.getX(), weapon.getY(), weapon.getWidth() / 2, weapon.getHeight() / 2, weapon.getWidth(), weapon.getHeight(), 1, 1, weapon.getAngle());

        for (Enemy enemy : enemies){
            enemy.draw(batch);
        }

        for (Bullet bullet : bullets){
            bullet.draw(batch, bullet.getX(), bullet.getY(), bullet.getWidth() / 2, bullet.getHeight() / 2, bullet.getWidth(), bullet.getHeight(), 2,2, bullet.getAngle());
        }

        for(Item item : items){
            item.draw(batch);
        }

        player.draw(batch);
    }

    public float getMapSizeWidth(){
        return mapSizeWidth;
    }

    public float getMapSizeHeight(){
        return mapSizeHeight;
    }

    public void createTextureRegion(String imagePath){
        textureRegion = new TextureRegion(new Texture(imagePath));
    }

    public int getScore(){
        return this.score;
    }
}
