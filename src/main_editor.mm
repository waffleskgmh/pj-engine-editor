#import <Cocoa/Cocoa.h>
#import <QuartzCore/QuartzCore.h>
#include "editor/editor_app.h"
#include "editor/scene_view.h"
#include "editor/game_view.h"
#include "editor/hierarchy_panel.h"
#include "editor/inspector_panel.h"
#include "editor/code_editor_panel.h"
#include <vector>
#include <memory>

using namespace PJEngine;

// Vlastný natívny macOS View s kreslením pixelov a zachytávaním myši/klávesnice
@interface StudioView : NSView {
    EditorApp* app;
    NSTimer* timer;
    std::vector<uint32_t> pixelBuffer;
    int bufWidth;
    int bufHeight;
    NSPoint lastMousePos;
    BOOL isDragging;
}
- (instancetype)initWithApp:(EditorApp*)editorApp;
@end

@implementation StudioView

- (instancetype)initWithApp:(EditorApp*)editorApp {
    self = [super init];
    if (self) {
        app = editorApp;
        bufWidth = 1280;
        bufHeight = 720;
        pixelBuffer.resize(bufWidth * bufHeight, 0xFF14171D); // Tmavé pozadie
        isDragging = NO;

        // 60 FPS herná slučka
        timer = [NSTimer scheduledTimerWithTimeInterval:1.0/60.0
                                                 target:self
                                               selector:@selector(gameLoop)
                                               userInfo:nil
                                                repeats:YES];
    }
    return self;
}

- (BOOL)acceptsFirstResponder { return YES; }

// Hlavná slučka editora a hry (60 FPS)
- (void)gameLoop {
    if (app->playState == PlayState::PLAYING) {
        app->stepFrame(1.0f / 60.0f);
    }
    [self setNeedsDisplay:YES];
}

// Kreslenie do natívneho okna macOS
- (void)drawRect:(NSRect)dirtyRect {
    (void)dirtyRect;
    CGContextRef ctx = [[NSGraphicsContext currentContext] CGContext];
    NSRect bounds = [self bounds];
    float w = bounds.size.width;
    float h = bounds.size.height;

    // 1. Pozadie editora
    CGContextSetRGBFillColor(ctx, 0.08, 0.09, 0.11, 1.0);
    CGContextFillRect(ctx, bounds);

    // 2. Kreslenie mriežky v Scene View
    CGContextSetRGBStrokeColor(ctx, 0.14, 0.16, 0.20, 1.0);
    CGContextSetLineWidth(ctx, 1.0);
    for (float x = 200.0; x < w - 250.0; x += 32.0) {
        CGContextMoveToPoint(ctx, x, 40.0);
        CGContextAddLineToPoint(ctx, x, h - 45.0);
    }
    for (float y = 40.0; y < h - 45.0; y += 32.0) {
        CGContextMoveToPoint(ctx, 200.0, y);
        CGContextAddLineToPoint(ctx, w - 250.0, y);
    }
    CGContextStrokePath(ctx);

    // 3. Kreslenie entít z ECSWorld (Hráč a Podlaha)
    auto entities = app->world.query<Transform2D, Collider2D>();
    for (Entity e : entities) {
        const auto& t = app->world.getComponent<Transform2D>(e);
        const auto& c = app->world.getComponent<Collider2D>(e);

        float screenX = 200.0 + t.position.x;
        float screenY = (h - 45.0) - t.position.y; // Obrátená os Y pre macOS Cocoa

        BOOL isSelected = (e == app->selectedEntity);

        if (c.type == ColliderType::CIRCLE) {
            // Hráč
            if (isSelected) {
                CGContextSetRGBFillColor(ctx, 0.23, 0.51, 0.96, 1.0); // Modrá
            } else {
                CGContextSetRGBFillColor(ctx, 0.4, 0.5, 0.6, 1.0);
            }
            CGContextFillEllipseInRect(ctx, CGRectMake(screenX - c.radius, screenY - c.radius, c.radius * 2, c.radius * 2));

            // Gizmo šípky na vybranej entite
            if (isSelected) {
                // Červená os X
                CGContextSetRGBStrokeColor(ctx, 0.94, 0.27, 0.27, 1.0);
                CGContextSetLineWidth(ctx, 3.0);
                CGContextMoveToPoint(ctx, screenX, screenY);
                CGContextAddLineToPoint(ctx, screenX + 40.0, screenY);
                CGContextStrokePath(ctx);

                // Zelená os Y
                CGContextSetRGBStrokeColor(ctx, 0.13, 0.77, 0.37, 1.0);
                CGContextMoveToPoint(ctx, screenX, screenY);
                CGContextAddLineToPoint(ctx, screenX, screenY - 40.0);
                CGContextStrokePath(ctx);
            }
        } else {
            // Podlaha
            CGContextSetRGBFillColor(ctx, 0.3, 0.35, 0.42, 1.0);
            CGContextFillRect(ctx, CGRectMake(screenX - c.size.x * 0.5, screenY - c.size.y * 0.5, c.size.x, c.size.y));
        }
    }

    // 4. Horná lišta (Toolbar)
    CGContextSetRGBFillColor(ctx, 0.11, 0.13, 0.16, 1.0);
    CGContextFillRect(ctx, CGRectMake(0, h - 42.0, w, 42.0));
    CGContextSetRGBStrokeColor(ctx, 0.18, 0.21, 0.26, 1.0);
    CGContextMoveToPoint(ctx, 0, h - 42.0);
    CGContextAddLineToPoint(ctx, w, h - 42.0);
    CGContextStrokePath(ctx);

    // 5. Ľavý panel (Hierarchia entít)
    CGContextSetRGBFillColor(ctx, 0.10, 0.12, 0.15, 1.0);
    CGContextFillRect(ctx, CGRectMake(0, 0, 200.0, h - 42.0));
    CGContextMoveToPoint(ctx, 200.0, 0);
    CGContextAddLineToPoint(ctx, 200.0, h - 42.0);
    CGContextStrokePath(ctx);

    // 6. Pravý panel (Inšpektor vlastností)
    CGContextFillRect(ctx, CGRectMake(w - 250.0, 0, 250.0, h - 42.0));
    CGContextMoveToPoint(ctx, w - 250.0, 0);
    CGContextAddLineToPoint(ctx, w - 250.0, h - 42.0);
    CGContextStrokePath(ctx);

    // Textové popisy v natívnom okne
    NSDictionary* titleAttr = @{
        NSFontAttributeName: [NSFont boldSystemFontOfSize:13],
        NSForegroundColorAttributeName: [NSColor colorWithCalibratedRed:0.23 green:0.51 blue:0.96 alpha:1.0]
    };
    [@"PJ STUDIO 2D (C++ / Native)" drawAtPoint:NSMakePoint(16, h - 28) withAttributes:titleAttr];

    NSDictionary* labelAttr = @{
        NSFontAttributeName: [NSFont systemFontOfSize:11],
        NSForegroundColorAttributeName: [NSColor whiteColor]
    };

    NSString* modeStr = (app->playState == PlayState::PLAYING) ? @"STAV: [PLAY MODE BEZI]" : @"STAV: [REZIM EDITACIE]";
    [modeStr drawAtPoint:NSMakePoint(w * 0.5 - 70, h - 28) withAttributes:labelAttr];

    [@"HIERARCHIA (ECS)" drawAtPoint:NSMakePoint(14, h - 65) withAttributes:titleAttr];
    [@"• Hrac (Player) [ID: 0]" drawAtPoint:NSMakePoint(14, h - 90) withAttributes:labelAttr];
    [@"• Podlaha (Ground) [ID: 1]" drawAtPoint:NSMakePoint(14, h - 110) withAttributes:labelAttr];

    [@"INSPEKTOR VLASTNOSTI" drawAtPoint:NSMakePoint(w - 235, h - 65) withAttributes:titleAttr];
    if (app->selectedEntity != NULL_ENTITY && app->world.hasComponent<Transform2D>(app->selectedEntity)) {
        const auto& t = app->world.getComponent<Transform2D>(app->selectedEntity);
        NSString* posStr = [NSString stringWithFormat:@"Pozicia X: %.1f\nPozicia Y: %.1f", t.position.x, t.position.y];
        [posStr drawAtPoint:NSMakePoint(w - 235, h - 110) withAttributes:labelAttr];
        [@"Ovladanie v Play Mode:\nSipky / WASD -> pohyb cez PJ" drawAtPoint:NSMakePoint(w - 235, h - 180) withAttributes:labelAttr];
    }
}

// Interakcia myšou (Ťahanie Gizmo šípok a posun entity)
- (void)mouseDown:(NSEvent *)event {
    lastMousePos = [event locationInWindow];
    isDragging = YES;
}

- (void)mouseDragged:(NSEvent *)event {
    if (!isDragging) return;
    NSPoint currentPos = [event locationInWindow];
    float dx = currentPos.x - lastMousePos.x;
    float dy = currentPos.y - lastMousePos.y;
    lastMousePos = currentPos;

    // Posun vybranej entity cez myš
    if (app->selectedEntity != NULL_ENTITY && app->world.hasComponent<Transform2D>(app->selectedEntity)) {
        auto& t = app->world.getComponent<Transform2D>(app->selectedEntity);
        t.position.x += dx;
        t.position.y -= dy; // Invert pre herný súradnicový systém
    }
}

- (void)mouseUp:(NSEvent *)event {
    (void)event;
    isDragging = NO;
}

// Ovládanie klávesnicou v Play Mode (riadi herný skript v PJ)
- (void)keyDown:(NSEvent *)event {
    unsigned short code = [event keyCode];
    if (code == 123 || code == 0) app->engineCtx.setKeyDown("left", true);  // Šípka vľavo / A
    if (code == 124 || code == 2) app->engineCtx.setKeyDown("right", true); // Šípka vpravo / D
    if (code == 126 || code == 13) app->engineCtx.setKeyDown("up", true);   // Šípka hore / W
    if (code == 125 || code == 1) app->engineCtx.setKeyDown("down", true);  // Šípka dole / S

    // Medzerník prepína Play Mode
    if (code == 49) {
        if (app->playState == PlayState::PLAYING) {
            app->setPlayState(PlayState::EDITING);
        } else {
            app->setPlayState(PlayState::PLAYING);
        }
    }
}

- (void)keyUp:(NSEvent *)event {
    unsigned short code = [event keyCode];
    if (code == 123 || code == 0) app->engineCtx.setKeyDown("left", false);
    if (code == 124 || code == 2) app->engineCtx.setKeyDown("right", false);
    if (code == 126 || code == 13) app->engineCtx.setKeyDown("up", false);
    if (code == 125 || code == 1) app->engineCtx.setKeyDown("down", false);
}

@end

// Vstupný bod natívnej macOS aplikácie
int main(int argc, const char * argv[]) {
    (void)argc;
    (void)argv;
    @autoreleasepool {
        [NSApplication sharedApplication];
        [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];

        EditorApp editorApp;

        NSRect frame = NSMakeRect(100, 100, 1280, 720);
        NSWindow* window = [[NSWindow alloc] initWithContentRect:frame
                                                       styleMask:(NSWindowStyleMaskTitled |
                                                                  NSWindowStyleMaskClosable |
                                                                  NSWindowStyleMaskMiniaturizable |
                                                                  NSWindowStyleMaskResizable)
                                                         backing:NSBackingStoreBuffered
                                                           defer:NO];

        [window setTitle:@"PJ Studio 2D - Natívny Herný Engine & Editor"];
        [window setBackgroundColor:[NSColor colorWithCalibratedRed:0.08 green:0.09 blue:0.11 alpha:1.0]];

        StudioView* view = [[StudioView alloc] initWithApp:&editorApp];
        [window setContentView:view];
        [window makeFirstResponder:view];
        [window makeKeyAndOrderFront:nil];

        [NSApp activateIgnoringOtherApps:YES];
        [NSApp run];
    }
    return 0;
}